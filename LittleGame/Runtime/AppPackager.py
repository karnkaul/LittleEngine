import datetime
import glob
import json
import os
import sys
import zipfile

# Vars
manifest_path = './AppManifest.json'
out_path = './Redist'
cooker_script = './AssetCooker.py'

# Execution
class Target:
    def __init__(self, source, dest):
        self.source = source
        self.dest = dest

class Manifest:
    def __init__(self):
        self.zip_path = ''
        self.zip_suffix = ''
        self.zip_ext = ''
        self.targets_to_zip = []
    
    def deserialise(self, json_str):
        data = json.loads(json_str)
        self.zip_path = data['zipName']
        self.zip_ext = data['zipExtension']
        dirs_data = data['directories']
        for dir_data in dirs_data:
            source_dir = dir_data['source']
            dest_dir = dir_data['destination']
            for dirname, subdirs, files in os.walk(source_dir):
                for filename in files:
                    parent_dir = dirname.replace(source_dir, dest_dir).replace('\\', '/')
                    self.targets_to_zip.append(Target(dirname + '/' + filename, parent_dir + '/' + filename))
        files_data = data['files']
        for file_data in files_data:
            self.targets_to_zip.append(Target(file_data['source'], file_data['destination']))

    def set_suffix(self, suffix):
        self.zip_suffix = suffix

    def get_zip_path(self, parent_dir):
        if parent_dir and not parent_dir.endswith('/'):
            parent_dir += '/'
        return parent_dir + self.zip_path + self.zip_suffix + self.zip_ext

def sizeof_fmt(num, suffix='B'):
    for unit in ['','Ki','Mi','Gi','Ti','Pi','Ei','Zi']:
        if abs(num) < 1024.0:
            return '%3.1f%s%s' % (num, unit, suffix)
        num /= 1024.0
    return '%.1f%s%s' % (num, 'Yi', suffix)

def init(manifest_path, manifest):
    if (not os.path.isfile(manifest_path)):
        print('  ERROR: Missing ' + manifest_path)
        sys.exit(-1)
    with (open(manifest_path, 'r')) as f:
        manifest.deserialise(f.read())
    for arg in sys.argv:
        if (arg == '-c' or arg == '--cook'):
            if os.path.isfile(cooker_script):
                os.system('python ' + cooker_script + ' -s')
            else:
                print('\n  ERROR: Cannot cook! ' + cooker_script + ' not found!')
                sys.exit(-1)
        if (arg.startswith('-v')):
            manifest.set_suffix(arg)

def create_package(zip_path, targets_to_zip):
    if os.path.isfile(zip_path):
        os.system('mv ' + zip_path + ' ' + zip_path + '.bak')
    column_width=0
    for target in targets_to_zip:
        if (len(target.dest) > column_width):
            column_width = len(target.dest)
    column_width += 5
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED, True, 9) as archive:
        for target in targets_to_zip:
            size_text = sizeof_fmt(os.path.getsize(target.source)).ljust(10)
            print(('\t' + target.dest).ljust(column_width) + size_text + ' ...added')
            archive.write(target.source, target.dest)
        size_text = ' [' + sizeof_fmt(os.path.getsize(zip_path)) + ']'
        print('\n  ' + zip_path + size_text + ' created successfully.')

def run():
    global manifest_path, out_path
    manifest = Manifest()
    if not os.path.isdir(out_path):
        os.mkdir(out_path)
    init(manifest_path, manifest)
    zip_path = manifest.get_zip_path(out_path)
    now = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M'))
    print('\n\t\t  APP PACKAGER\n\t\t' + now + '\n')
    print(('  Manifest').ljust(15) + ': ' + manifest_path)
    print(('  Output').ljust(15) + ': ' + zip_path + '\n')
    create_package(zip_path, manifest.targets_to_zip)

if __name__ == "__main__":
    run()
