import json
import os
import sys
import zipfile

# Vars
out_path = '../Redist'
cooker_script = './AssetCooker.py'

# Execution
class Target:
	def __init__(self, source, dest, present):
		self.source = source
		self.dest = dest
		self.present = present

class Manifest:
	def __init__(self):
		self.zip_path = ''
		self.zip_suffix = ''
		self.zip_ext = ''
		self.method = zipfile.ZIP_STORED
		self.targets_to_zip = []
		
	def deserialise(self, json_str):
		required_missing = False
		dirs_not_found=[]
		files_not_found=[]
		data = json.loads(json_str)
		self.zip_path = data['zipName']
		self.zip_ext = data['zipExtension']
		if ('compress' in data):
			if data['compress']:
				self.method = zipfile.ZIP_DEFLATED
		if ('directories' in data):
			dirs_data = data['directories']
			for dir_data in dirs_data:
				source_dir = dir_data['source']
				dest_dir = dir_data['destination']
				required = False
				present = True
				if ('isRequired' in dir_data):
					required = dir_data['isRequired']
				if (not os.path.isdir(source_dir)):
					dirs_not_found.append(Target(source_dir, dest_dir, False))
					present = False
					if required:
						required_missing = True
				for dirname, subdirs, files in os.walk(source_dir):
					for filename in files:
						parent_dir = dirname.replace(source_dir, dest_dir).replace('\\', '/')
						target = Target(dirname + '/' + filename, parent_dir + '/' + filename, present)
						self.targets_to_zip.append(target)
		if ('files' in data):
			files_data = data['files']
			for file_data in files_data:
				required = False
				present = True
				if ('isRequired' in file_data):
					required = file_data['isRequired']
				target = Target(file_data['source'], file_data['destination'], present)
				if (not os.path.isfile(target.source)):
					target.present = False
					files_not_found.append(target)
					if required:
						required_missing = True
				self.targets_to_zip.append(target)
			if (files_not_found or dirs_not_found):
				prologue = 'WARNING'
				if required_missing:
					prologue = 'ERROR'
				print ('\n  ' + prologue + ': Manifest items not on disk')
				if (files_not_found):
					print('\n\tFiles not found:')
					for not_found in files_not_found:
						print(('\t  ' + not_found.source).ljust(40) + ' => ' + not_found.dest)
				if (dirs_not_found):
					print('\n\tDirectories not found:')
					for not_found in dirs_not_found:
						print(('\t  ' + not_found.source).ljust(40) + ' => ' + not_found.dest)
				print('')
		return not required_missing

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
	print('\n  ## Packaging using ' + manifest_path + '...')
	error = False
	if (not os.path.isfile(manifest_path)):
		print('  ERROR: Missing ' + manifest_path)
		error = True
		return False
	with (open(manifest_path, 'r')) as f:
		if (not manifest.deserialise(f.read())):
			error = True
	if not error:
		print(('  Manifest').ljust(15) + ': ' + manifest_path)
	return not error

def create_package(zip_path, targets_to_zip, method):
	print(('  Output').ljust(15) + ': ' + zip_path + '\n')
	if os.path.isfile(zip_path):
		os.system('mv ' + zip_path + ' ' + zip_path + '.bak')
	column_width=0
	for target in targets_to_zip:
		if (len(target.dest) > column_width):
			column_width = len(target.dest)
	column_width += 5
	with zipfile.ZipFile(zip_path, 'w', method, True) as archive:
		suffix = ' created successfully.'
		for target in targets_to_zip:
			if not target.present:
				suffix = ' created but is missing some files!'
				continue
			size_text = sizeof_fmt(os.path.getsize(target.source)).ljust(10)
			print(('\t' + target.dest).ljust(column_width) + size_text + ' ...added')
			archive.write(target.source, target.dest)
		size_text = ' [' + sizeof_fmt(os.path.getsize(zip_path)) + ']'
		print('\n  ' + zip_path + size_text + suffix)

def call(manifest_path, out_path, version_str=''):
	manifest = Manifest()
	if version_str.startswith('-v'):
		manifest.set_suffix(version_str)
	if not os.path.isdir(out_path):
		os.makedirs(out_path)
	if (not init(manifest_path, manifest)):
		print('\n  FATAL ERROR(s)! Aborting package!')
		return -1
	zip_path = manifest.get_zip_path(out_path)
	create_package(zip_path, manifest.targets_to_zip, manifest.method)
	return 0

def run():
	if len(sys.argv) < 2:
		print('Need Manifest path passed as argument')
		return -1
	manifest_path=sys.argv[1]
	version_str = ''
	for arg in sys.argv:
		if (arg.startswith('-v')):
			version_str = arg
	global out_path
	return call(manifest_path, out_path, version_str)

if __name__ == '__main__':
	run()
