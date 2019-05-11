import glob
import json
import os
import shutil
import sys
import zipfile

# Vars
manifest_path = './AppManifest.json'
out_path = './Redist'

# Execution
class Target:
    def __init__(self, source, dest):
        self.source = source
        self.dest = dest

class Manifest:
    def __init__(self):
        self.zip_name = ''
        self.zip_suffix = ''
        self.zip_ext = ''
        self.targets_to_zip = []
    
    def deserialise(self, json_str):
        data = json.loads(json_str)
        self.zip_name = data['zipName']
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
        return parent_dir + self.zip_name + self.zip_suffix + self.zip_ext

def init(manifest_path, manifest):
    if (not os.path.isfile(manifest_path)):
        print('  ERROR: Missing ' + manifest_path)
        sys.exit(-1)
    with (open(manifest_path, 'r')) as f:
        manifest.deserialise(f.read())
    for arg in sys.argv:
        if (arg.startswith('-v')):
            manifest.set_suffix(arg)

def create_package(zip_name, targets_to_zip):
    if os.path.isfile(zip_name):
        os.system('mv ' + zip_name + ' ' + zip_name + '.bak')
    column_width=0
    for target in targets_to_zip:
        if (len(target.dest) > column_width):
            column_width = len(target.dest)
    column_width += 3
    with zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED, True, 9) as archive:
        for target in targets_to_zip:
            print(('\t' + target.dest).ljust(column_width) + ' ...added')
            archive.write(target.source, target.dest)
        print('\n  ' + zip_name + ' packaged successfully.')

def run():
    global manifest_path, out_path
    manifest = Manifest()
    init(manifest_path, manifest) 
    create_package(manifest.get_zip_path(out_path), manifest.targets_to_zip)

if __name__ == "__main__":
    run()
