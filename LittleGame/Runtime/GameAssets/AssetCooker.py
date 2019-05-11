from enum import Enum
import datetime
import json
import os
import sys
import zipfile

# Cook vars
g_cooked = '../GameAssets.cooked'
g_manifest = './Manifest.amf'

# Globals
class Mode(Enum):
    Analyse     = 0
    Cook_Prompt = 1
    Cook_Silent = 2

class Asset:
    def __init__(self, _type, path, _bytes):
        self._type = _type
        self.path = path
        self._bytes = _bytes    

g_asset_type_map = {}
g_asset_type_map['textures'] = 'Texture'
g_asset_type_map['sounds'] = 'Sound'
g_asset_type_map['fonts'] = 'Font'
g_asset_type_map['texts'] = 'Text'
g_asset_type_map['textureSets'] = 'Texture'
g_asset_type_map['soundSets'] = 'Sound'

g_mode = Mode.Analyse
g_backup = ''
g_log_dashes = 80
g_log_str = ''
g_bCooked_exists = False

g_in_manifest = []
g_not_on_fs = []
g_in_cooked = []
g_only_in_cooked = []
g_not_in_cooked = []
g_to_cook = []

# Utility
def log(_str):
    global g_log_str
    g_log_str += (_str + '\n')
    print(_str)

def log_dashes(_str = ''):
    global g_log_dashes
    log('  ' + ('-' * g_log_dashes))
    if _str:
        log('  ' + _str)
        log('  ' + ('-' * g_log_dashes))

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
            
def log_assets_if_not_empty(_type, assets, bPrint_size):
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
            total_size += asset._bytes
            path_text = asset.path.ljust(column_width)
            size_text = sizeof_fmt(asset._bytes)
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
        if (asset._type == 'Font'):
            fonts.append(asset)
        elif (asset._type == 'Sound'):
            sounds.append(asset)
        elif (asset._type == 'Text'):
            texts.append(asset)
        else:
            textures.append(asset)
    total_size = 0
    total_size += log_assets_if_not_empty('FONTS', fonts, bPrint_size)
    total_size += log_assets_if_not_empty('SOUNDS', sounds, bPrint_size)
    total_size += log_assets_if_not_empty('TEXTS', texts, bPrint_size)
    total_size += log_assets_if_not_empty('TEXTURES', textures, bPrint_size)
    return total_size

def log_asset_flat(assets, bPrint_size = True):
    return log_assets_if_not_empty('ASSETS', assets, bPrint_size)

# Init
def init():
    global g_manifest, g_bCooked_exists, g_mode, g_cooked, g_backup
    prev_arg = ''
    for arg in sys.argv:
        if (prev_arg == '--manifest' or prev_arg == '-m'):
            g_manifest = arg
        elif (prev_arg == '--zip' or prev_arg == '-z'):
            g_cooked = arg
        if (arg == '--cook' or arg == '-c'):
            g_mode = Mode.Cook_Prompt
        elif (arg == '--silent' or arg == '-s'):
            g_mode = Mode.Cook_Silent
        prev_arg = arg
    g_bCooked_exists = os.path.isfile(g_cooked)
    if g_bCooked_exists:
        g_backup = g_cooked + '.bak'

# Operations
def log_header():
    global g_mode, g_bCooked_exists, g_cooked, g_backup
    now = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M'))
    log('\n\t\tLITTLE ENGINE ASSET COOKER\n\t\t  ' + now + '\n')
    cw = 17
    if (not os.path.isfile(g_manifest)):
        log ('  ERROR! Missing manifest [' + g_manifest + ']')
        sys.exit(-1)
    log(('  Manifest').ljust(cw) + ': ' + g_manifest)
    if (g_mode == Mode.Analyse):
        if (g_bCooked_exists):
            log(('  Analysed').ljust(cw) + ': ' + g_cooked)
        else:
            log(('  Cook Target').ljust(cw) + ': '  + g_cooked)
    elif (g_mode == Mode.Cook_Prompt):
        if (g_bCooked_exists):
            log(('  Backing Up To').ljust(cw) + ': ' + g_backup)
        log(('  Cooking To').ljust(cw) + ': ' + g_cooked)
    elif (g_mode == Mode.Cook_Silent):
        if (g_bCooked_exists):
            log(('  Backup').ljust(cw) + ': ' + g_backup)
        log(('  Cooked').ljust(cw) + ': ' + g_cooked)

            
def read_manifest():
    global g_in_manifest, g_manifest, g_asset_type_map
    with open(g_manifest, 'r') as file:
        loaded_manifest = json.loads(file.read())
        a_types = ['textures', 'fonts', 'sounds', 'texts']
        for a_type in a_types:
            if a_type in loaded_manifest:
                for path in loaded_manifest[a_type]:
                    if (os.path.isfile(path)):
                        g_in_manifest.append(Asset(g_asset_type_map[a_type], path, os.path.getsize(path)))
                    else:
                        g_not_on_fs.append(path)
    asset_sets = ['textureSets', 'soundSets']
    for asset_set in asset_sets:
        for asset_path in loaded_manifest[asset_set]:
            count = asset_path['count']
            for i in range(0, count):
                filename = str(i).zfill(2)
                path_prefix = asset_path['pathPrefix']
                if path_prefix:
                    path_prefix += '/'
                path = path_prefix + asset_path['assetPrefix'] + filename + asset_path['assetSuffix']
                if (os.path.isfile(path)):
                    g_in_manifest.append(Asset(g_asset_type_map[asset_set], path, os.path.getsize(path)))
                else:
                    g_not_on_fs.append(path)

def populate():
    global g_bCooked_exists, g_cooked, g_manifest, g_in_manifest, g_in_cooked, g_not_in_cooked, g_to_cook, g_only_in_cooked
    if (g_bCooked_exists):
        with zipfile.ZipFile(g_cooked, 'r') as archive:
                for asset in g_in_manifest:
                    if asset.path in archive.namelist():
                        g_in_cooked.append(asset)
                    else:
                        g_not_in_cooked.append(asset)
                manifest_name = g_manifest
                while (manifest_name.startswith('/') or manifest_name.startswith('.')):
                    manifest_name = manifest_name[1:]
                for name in archive.namelist():
                    found = False
                    for asset in g_in_manifest:
                        if (name == asset.path):
                            found = True
                            break
                    if (not found) and (not name.endswith('/')) and (not name == manifest_name):
                        g_only_in_cooked.append(name)
    else:
        g_not_in_cooked = g_in_manifest.copy()
    g_to_cook = g_in_manifest.copy()
    g_to_cook.append(Asset('Text', g_manifest, os.path.getsize(g_manifest)))
    g_in_manifest.sort(key = lambda x: x._type)
    g_in_cooked.sort(key = lambda x: x._type)
    g_not_in_cooked.sort(key=lambda x: x._type)
    g_to_cook.sort(key=lambda x: x._type)

def cook(paths):
    global g_cooked, g_manifest, g_backup
    if (g_cooked and g_backup):
        os.system('cp ' + g_cooked + ' ' + g_backup)
    with zipfile.ZipFile(g_cooked, 'w', zipfile.ZIP_DEFLATED) as zipF:
        for path in paths:
            zipF.write(path, path)
            print('\tCooked ' + path)
        print('')

def log_summary():
    global g_mode, g_manifest, g_cooked, g_in_manifest, g_to_cook, g_in_cooked, g_only_in_cooked
    # Manifest Crawl
    log('  \n\t\t\tMANIFEST CRAWL')
    log_asset_group_by_type(g_in_manifest)
    
    # Missing files
    if (g_not_on_fs):
        log('  ')
        log_paths(g_not_on_fs, '  ERROR: ASSETS MISSING FROM FILESYSTEM!')

    # Cook List
    if (g_mode == Mode.Analyse and g_in_cooked):
        if (g_bCooked_exists and g_not_in_cooked):
            log('  \n\t\t\tNEW ASSETS')
            log_asset_group_by_type(g_not_in_cooked)
        log('  \n\t\t\tEXISTING ASSETS (will be overwritten)')
        log_asset_group_by_type(g_in_cooked, False)
    if (g_mode == Mode.Cook_Silent):
        header = '  \n\t\t\tASSETS COOKED'
    else:
        header = '  \n\t\t\tASSETS TO COOK'
    log(header)
    log_asset_flat(g_to_cook)    

    # Orphaned
    if (g_only_in_cooked):
        log('')
        log_paths(g_only_in_cooked, '  ORPHANED ASSETS (will no longer be in ' + g_cooked + ')')

    if (g_not_on_fs):
        if (g_mode == Mode.Analyse):
            log('\n  ERROR: Invalid manifest entries / missing files! Attempt to cook will fail!')
        else:
            log('\n  ERROR: Invalid manifest entries / missing files! Cook aborted!')
        return False
    return True

def sizeof_file(path):
    return ' [' + sizeof_fmt(os.path.getsize(path)) + ']'

def run():
    global g_mode, g_log_str
    init()
    log_header()
    read_manifest()
    populate()
    if (log_summary()):
        if (g_mode == Mode.Cook_Silent):
            bCook = True
        elif (g_mode == Mode.Cook_Prompt):
            bCook = False
            prompt = input('  Proceed with cooking (Run with -s / --silent to suppress)? [n]: ')
            if (prompt == 'y'):
                bCook = True
        else:
            bCook = False
        if (g_mode != Mode.Analyse and not bCook):
            log('  Cook Aborted.')
        if (bCook):
            paths = []
            for asset in g_to_cook:
                paths.append(asset.path)
            cook(paths)
            log(('  Cook Completed').ljust(20) + ': ' + g_cooked + sizeof_file(g_cooked))
            if (g_backup):
                log(('  Back up').ljust(20) + ': ' + g_backup + sizeof_file(g_backup))
        if (g_mode == Mode.Analyse):
            log(('  Crawled').ljust(20) + ': ' + g_manifest)
            if (g_bCooked_exists):
                log(('  Analysed').ljust(20) + ': ' + g_cooked + sizeof_file(g_cooked))
            suffix = ' '
            if (os.path.isfile(g_cooked)):
                suffix = ' and overwrite '
            print('\n  Analysis complete. Run again with -c (--cook) to generate' + suffix + g_cooked)
    with open('AssetCooker.report', 'w') as file:
        file.write(g_log_str)
        g_log_str = ''

if (__name__ == '__main__'):
    run()