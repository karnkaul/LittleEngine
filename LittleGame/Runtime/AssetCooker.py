from enum import Enum
import datetime
import json
import os
import sys
import zipfile

# Cook vars
manifest_name = 'Manifest.amf'
assets_root_dir = './GameAssets'
cooked_path = './GameAssets.cooked'

class Mode(Enum):
    Analyse     = 0
    Cook_Prompt = 1
    Cook_Silent = 2

class Asset:
    def __init__(self, _type, path, _id):
        self.type = _type
        self.path = path
        self.id = _id
        self.bytes = os.path.getsize(self.path)

class Manifest:
    def __init__(self, manifest_name, assets_root_dir):
        self.assets_root_dir = assets_root_dir
        self.manifest_name = manifest_name
        self.manifest_path = assets_root_dir + '/' + manifest_name
        self.cooked_path = ''
        self.assets_in_manifest = []
        self.assets_not_on_fs = []
        self.assets_in_cooked = []
        self.assets_only_in_cooked = []
        self.assets_not_in_cooked = []
        self.assets_to_cook = []
        asset_type_map = {}
        asset_type_map['textures'] = 'Texture'
        asset_type_map['sounds'] = 'Sound'
        asset_type_map['fonts'] = 'Font'
        asset_type_map['texts'] = 'Text'
        with open(self.manifest_path, 'r') as file:
            loaded_manifest = json.loads(file.read())
            a_types = ['textures', 'fonts', 'sounds', 'texts']
            for a_type in a_types:
                if a_type in loaded_manifest:
                    for asset_id in loaded_manifest[a_type]:
                        asset_path = self.assets_root_dir + '/' + asset_id
                        if (os.path.isfile(asset_path)):
                            self.assets_in_manifest.append(Asset(asset_type_map[a_type], asset_path, asset_id))
                        else:
                            self.assets_not_on_fs.append(asset_path)
        asset_set_ids = ['textureSets', 'soundSets']
        for asset_set_id in asset_set_ids:
            a_type = asset_set_id.replace('Set', '')
            for asset_set in loaded_manifest[asset_set_id]:
                count = asset_set['count']
                for i in range(0, count):
                    filename = str(i).zfill(2)
                    path_prefix = asset_set['pathPrefix'] + '/' if 'pathPrefix' in asset_set else ''
                    asset_prefix = asset_set['assetPrefix'] if 'assetPrefix' in asset_set else ''
                    asset_suffix = asset_set['assetSuffix'] if 'assetSuffix' in asset_set else ''
                    asset_id = path_prefix + asset_prefix + filename + asset_suffix
                    asset_path = self.assets_root_dir + '/' + asset_id
                    if (os.path.isfile(asset_path)):
                        self.assets_in_manifest.append(Asset(asset_type_map[a_type], asset_path, asset_id))
                    else:
                        self.assets_not_on_fs.append(asset_path)

    def load(self, cooked_path):
        self.cooked_path = cooked_path
        if (os.path.isfile(self.cooked_path)):
            with zipfile.ZipFile(self.cooked_path, 'r') as archive:
                    for asset in self.assets_in_manifest:
                        if asset.id in archive.namelist():
                            self.assets_in_cooked.append(asset)
                        else:
                            self.assets_not_in_cooked.append(asset)
                    for asset_id in archive.namelist():
                        if (asset_id.endswith('/') or asset_id.endswith('\\')):
                            continue
                        found = False
                        for asset in self.assets_in_manifest:
                            if (asset_id == asset.id):
                                found = True
                                break
                        if (not found) and (not asset_id == self.manifest_name):
                            self.assets_only_in_cooked.append(asset_id)
        else:
            self.assets_not_in_cooked = self.assets_in_manifest.copy()
        self.assets_to_cook = self.assets_in_manifest.copy()
        self.assets_in_manifest.sort(key = lambda x: x.type)
        self.assets_in_cooked.sort(key = lambda x: x.type)
        self.assets_not_in_cooked.sort(key=lambda x: x.type)
        self.assets_to_cook.sort(key=lambda x: x.type)
        self.assets_to_cook.insert(0, Asset('Text', self.manifest_path, self.manifest_name))
        

mode = Mode.Analyse
bCooked_exists = False
backup_path = ''

log_str = ''

# Utility
def log(_str):
    global log_str
    log_str += (_str + '\n')
    print(_str)

def log_dashes(_str = ''):
    log_dash_count = 80
    log('  ' + ('-' * log_dash_count))
    if _str:
        log('  ' + _str)
        log('  ' + ('-' * log_dash_count))

def sizeof_fmt(num, suffix='B'):
    for unit in ['','Ki','Mi','Gi','Ti','Pi','Ei','Zi']:
        if abs(num) < 1024.0:
            return '%3.1f%s%s' % (num, unit, suffix)
        num /= 1024.0
    return '%.1f%s%s' % (num, 'Yi', suffix)

# Log Functions
def log_paths(paths, header = ''):
    if (header):
        log_dashes('\t\t' + header)
    i = 0
    column_width = 0
    for path in paths:
        if (len(path) > column_width):
            column_width = len(path)
    column_width += 5
    total_size = 0
    for path in paths:
        i += 1
        path_text = path.ljust(column_width)
        index_text = (str(i) + '.').ljust(4)
        log('\t' + index_text + path_text)
    log(' ')
            
def log_assets_if_not_empty(_type, assets, bPrint_size, bPrint_id = False):
    total_size = 0
    if (assets):
        log_dashes('\t\t' + _type)
        i = 0
        column_width = 0
        for asset in assets:
            if (len(asset.path) > column_width):
                column_width = len(asset.path)
        column_width += 5
        for asset in assets:
            i += 1
            total_size += asset.bytes
            path_text = asset.id.ljust(column_width) if bPrint_id else asset.path.ljust(column_width)
            size_text = sizeof_fmt(asset.bytes)
            index_text = (str(i) + '.').ljust(5)
            if (bPrint_size):
                log('\t' + index_text + path_text + size_text)
            else:
                log('\t' + index_text + path_text)
        if (bPrint_size):
            log(('\tTotal: ').ljust(column_width + 6) + sizeof_fmt(total_size))
        log(' ')
    return total_size

def log_asset_group_by_type(assets, bPrint_size = True):
    fonts = []
    sounds = []
    texts = []
    textures = []
    for asset in assets:
        if (asset.type == 'Font'):
            fonts.append(asset)
        elif (asset.type == 'Sound'):
            sounds.append(asset)
        elif (asset.type == 'Text'):
            texts.append(asset)
        else:
            textures.append(asset)
    total_size = 0
    total_size += log_assets_if_not_empty('FONTS', fonts, bPrint_size)
    total_size += log_assets_if_not_empty('SOUNDS', sounds, bPrint_size)
    total_size += log_assets_if_not_empty('TEXTS', texts, bPrint_size)
    total_size += log_assets_if_not_empty('TEXTURES', textures, bPrint_size)
    return total_size

def log_asset_flat(assets, bPrint_size = True, bPrint_id = False):
    return log_assets_if_not_empty('ASSETS', assets, bPrint_size, bPrint_id)

def get_sizeof_file(path):
    return ' [' + sizeof_fmt(os.path.getsize(path)) + ']'

# Init
def init():
    global manifest_name, assets_root_dir, bCooked_exists, mode, cooked_path, backup_path
    prev_arg = ''
    for arg in sys.argv:
        if (prev_arg == '--manifest' or prev_arg == '-m'):
            manifest_name = arg
        elif (prev_arg == '--zip' or prev_arg == '-z'):
            cooked_path = arg
        if (arg == '--cook' or arg == '-c'):
            mode = Mode.Cook_Prompt
        elif (arg == '--silent' or arg == '-s'):
            mode = Mode.Cook_Silent
        prev_arg = arg
    bCooked_exists = os.path.isfile(cooked_path)
    if bCooked_exists:
        backup_path = cooked_path + '.bak'
    now = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M'))
    log('\n\t\tLITTLE ENGINE ASSET COOKER\n\t\t  ' + now + '\n')
    cw = 17
    manifest_path = assets_root_dir + '/' + manifest_name
    if (not os.path.isfile(manifest_path)):
        log ('  ERROR! Missing manifest [' + manifest_path + ']')
        sys.exit(-1)
    log(('  Manifest').ljust(cw) + ': ' + manifest_path)
    if (mode == Mode.Analyse):
        if (bCooked_exists):
            log(('  Analysed').ljust(cw) + ': ' + cooked_path)
        else:
            log(('  Cook Target').ljust(cw) + ': '  + cooked_path)
    elif (mode == Mode.Cook_Prompt):
        if (bCooked_exists):
            log(('  Backing Up To').ljust(cw) + ': ' + backup_path)
        log(('  Cooking To').ljust(cw) + ': ' + cooked_path)
    elif (mode == Mode.Cook_Silent):
        if (bCooked_exists):
            log(('  Backup').ljust(cw) + ': ' + backup_path)
        log(('  Cooked').ljust(cw) + ': ' + cooked_path)

def cook(manifest, backup_path):
    if (manifest.cooked_path and backup_path):
        os.system('mv ' + manifest.cooked_path + ' ' + backup_path)
    with zipfile.ZipFile(manifest.cooked_path, 'w', zipfile.ZIP_DEFLATED, True, 9) as zipF:
        for asset in manifest.assets_to_cook:
            zipF.write(asset.path, asset.id)
            print('\tCooked ' + asset.id)
        print('')

def log_summary(manifest):
    global mode, assets_root_dir, manifest_name, cooked_path
    # Manifest Crawl
    log('  \n\t\t\tMANIFEST CRAWL')
    log_asset_group_by_type(manifest.assets_in_manifest)
    
    # Missing files
    if (manifest.assets_not_on_fs):
        log('  ')
        log_paths(manifest.assets_not_on_fs, '  ERROR: ASSETS MISSING FROM FILESYSTEM!')

    # Cook List
    if (mode == Mode.Analyse and manifest.assets_in_cooked):
        if (bCooked_exists and manifest.assets_not_in_cooked):
            log('  \n\t\t\tNEW ASSETS')
            log_asset_group_by_type(manifest.assets_not_in_cooked)
        log('  \n\t\t\tEXISTING ASSETS (will be overwritten)')
        log_asset_group_by_type(manifest.assets_in_cooked, False)
    if (mode == Mode.Cook_Silent):
        header = '  \n\t\t\tASSETS COOKED'
    else:
        header = '  \n\t\t\tASSETS TO COOK'
    log(header)
    log_asset_flat(manifest.assets_to_cook, True, True)

    # Orphaned
    if (manifest.assets_only_in_cooked):
        log('')
        log_paths(manifest.assets_only_in_cooked, '  ORPHANED ASSETS (will no longer be in ' + cooked_path + ')')

    if (manifest.assets_not_on_fs):
        if (mode == Mode.Analyse):
            log('\n  ERROR: Invalid manifest entries / missing files! Attempt to cook will fail!')
        else:
            log('\n  ERROR: Invalid manifest entries / missing files! Cook aborted!')
        return False
    return True

def run():
    global manifest_name, assets_root_dir, cooked_path, mode, log_str, backup_path
    init()
    manifest = Manifest(manifest_name, assets_root_dir)
    manifest.load(cooked_path)
    if (log_summary(manifest)):
        if (mode == Mode.Cook_Silent):
            bCook = True
        elif (mode == Mode.Cook_Prompt):
            bCook = False
            prompt = input('  Proceed with cooking (Run with -s / --silent to suppress)? [n]: ')
            if (prompt == 'y'):
                bCook = True
        else:
            bCook = False
        if (mode != Mode.Analyse and not bCook):
            log('  Cook Aborted.')
        if (bCook):
            cook(manifest, backup_path)
            log(('  Cook Completed').ljust(20) + ': ' + cooked_path + get_sizeof_file(cooked_path))
            if (backup_path):
                log(('  Back up').ljust(20) + ': ' + backup_path + get_sizeof_file(backup_path))
        if (mode == Mode.Analyse):
            log(('  Crawled').ljust(20) + ': ' + manifest.manifest_path)
            if (bCooked_exists):
                log(('  Analysed').ljust(20) + ': ' + cooked_path + get_sizeof_file(cooked_path))
            suffix = ' '
            if (os.path.isfile(cooked_path)):
                suffix = ' and overwrite '
            print('\n  Analysis complete. Run again with -c (--cook) to generate' + suffix + cooked_path)
    with open('AssetCooker.report', 'w') as file:
        file.write(log_str)
        log_str = ''

if (__name__ == '__main__'):
    run()