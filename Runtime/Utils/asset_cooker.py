from enum import Enum
import datetime
import glob
import json
import os
import sys
import zipfile

os.chdir(sys.path[0])

# Cook vars
manifest_ext = '.amf'
assets_root_dir = '../GameAssets'
cooked_path = '../GameAssets.cooked'

manifests = []
assets_in_manifests = []
assets_not_on_fs = []
assets_in_cooked = []
assets_only_in_cooked = []
assets_not_in_cooked = []
assets_to_cook = []

asset_type_map = {}
asset_type_map['textures'] = 'Texture'
asset_type_map['sounds'] = 'Sound'
asset_type_map['fonts'] = 'Font'
asset_type_map['texts'] = 'Text'

class Mode(Enum):
	Analyse = 0
	Cook = 1

mode = Mode.Analyse
cooked_exists = False
backup_path = ''

class Asset:
	def __init__(self, _type, path, _id):
		self.type = _type
		self.path = path
		self.id = _id
		self.bytes = os.path.getsize(self.path)
	def __eq__(self, other):
		if isinstance(other, Asset):
			return self.id == other.id
	def __hash__(self):
		return hash(self.id)

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
	for path in paths:
		i += 1
		path_text = path.ljust(column_width)
		index_text = (str(i) + '.').ljust(4)
		log('\t' + index_text + path_text)
	log(' ')
			
def log_assets_if_not_empty(_type, assets, do_print_size, bPrint_id = False):
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
			if (do_print_size):
				log('\t' + index_text + path_text + size_text)
			else:
				log('\t' + index_text + path_text)
		if (do_print_size):
			log(('\tTotal: ').ljust(column_width + 6) + sizeof_fmt(total_size))
		log(' ')
	return total_size

def log_asset_group_by_type(assets, do_print_size = True):
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
	total_size += log_assets_if_not_empty('FONTS', fonts, do_print_size)
	total_size += log_assets_if_not_empty('SOUNDS', sounds, do_print_size)
	total_size += log_assets_if_not_empty('TEXTS', texts, do_print_size)
	total_size += log_assets_if_not_empty('TEXTURES', textures, do_print_size)
	return total_size

def log_asset_flat(assets, do_print_size = True, bPrint_id = False):
	return log_assets_if_not_empty('ASSETS', assets, do_print_size, bPrint_id)

def get_sizeof_file(path):
	return ' [' + sizeof_fmt(os.path.getsize(path)) + ']'

# Args
def process_args():
	global manifest_name, cooked_path, mode
	prev_arg = ''
	for arg in sys.argv:
		if (prev_arg == '--manifest' or prev_arg == '-m'):
			manifest_name = arg
		elif (prev_arg == '--zip' or prev_arg == '-z'):
			cooked_path = arg
		if (arg == '--cook' or arg == '-c'):
			mode = Mode.Cook
		prev_arg = arg

# Core
def init():
	global cooked_exists, backup_path
	if (os.getcwd().endswith('Runtime')):
		os.chdir('Utils')
	cooked_exists = os.path.isfile(cooked_path)
	if cooked_exists:
		backup_path = cooked_path + '.bak'
	log('')
	cw = 17
	manifest_names = []
	for filename in glob.glob(assets_root_dir +  '/*' + manifest_ext):
		manifests.append(filename)
		manifest_names.append(os.path.basename(filename))
	if not manifests:
		log ('  ERROR! No manifests found in  [' + assets_root_dir + ']')
		return False
	log(('  Manifests:').ljust(cw) + ': ' + (', '.join(manifest_names)))
	if (mode == Mode.Analyse):
		if (cooked_exists):
			log(('  Analysed').ljust(cw) + ': ' + cooked_path)
		else:
			log(('  Cook Target').ljust(cw) + ': '  + cooked_path)
	elif (mode == Mode.Cook):
		if (cooked_exists):
			log(('  Backup').ljust(cw) + ': ' + backup_path)
		log(('  Cooking').ljust(cw) + ': ' + cooked_path)
	return True

def load_asset_manifest(manifest_path):
	manifest_name = os.path.basename(manifest_path)
	assets_in_manifest = []
	with open(manifest_path, 'r') as file:
		loaded_manifest = json.loads(file.read())
		a_types = ['textures', 'fonts', 'sounds', 'texts']
		for a_type in a_types:
			if a_type in loaded_manifest:
				for asset_id in loaded_manifest[a_type]:
					asset_path = assets_root_dir + '/' + asset_id
					if (os.path.isfile(asset_path)):
						assets_in_manifest.append(Asset(asset_type_map[a_type], asset_path, asset_id))
					else:
						assets_not_on_fs.append(asset_path)
	asset_set_ids = ['textureSets', 'soundSets']
	for asset_set_id in asset_set_ids:
		a_type = asset_set_id.replace('Set', '')
		if asset_set_id in loaded_manifest:
			for asset_set in loaded_manifest[asset_set_id]:
				count = asset_set['count']
				for i in range(0, count):
					filename = str(i).zfill(2)
					path_prefix = asset_set['pathPrefix'] + '/' if 'pathPrefix' in asset_set else ''
					asset_prefix = asset_set['assetPrefix'] if 'assetPrefix' in asset_set else ''
					asset_suffix = asset_set['assetSuffix'] if 'assetSuffix' in asset_set else ''
					asset_id = path_prefix + asset_prefix + filename + asset_suffix
					asset_path = assets_root_dir + '/' + asset_id
					if (os.path.isfile(asset_path)):
						assets_in_manifest.append(Asset(asset_type_map[a_type], asset_path, asset_id))
					else:
						assets_not_on_fs.append(asset_path)
		if (os.path.isfile(cooked_path)):
			with zipfile.ZipFile(cooked_path, 'r') as archive:
					for asset in assets_in_manifest:
						if asset.id in archive.namelist():
							assets_in_cooked.append(asset)
						else:
							assets_not_in_cooked.append(asset)
		else:
			assets_not_in_cooked.extend(assets_in_manifest)
	assets_to_cook.append(Asset('Manifest', manifest_path, manifest_name))
	assets_in_manifests.extend(assets_in_manifest)

def log_summary():
	# Manifest Crawl
	log('  \n\t\t\tASSETS IN MANIFEST(s)')
	total = log_asset_group_by_type(assets_in_manifests)
	log('\tSum Total: ' + sizeof_fmt(total))
	# Missing files
	if (assets_not_on_fs):
		log('  ')
		log_paths(assets_not_on_fs, '  ERROR: ASSETS MISSING FROM FILESYSTEM!')
	# Orphaned
	if (assets_only_in_cooked):
		log('')
		log_paths(assets_only_in_cooked, '  ORPHANED ASSETS (will no longer be in ' + cooked_path + ')')
	# Cook List
	if mode == Mode.Analyse:
		if assets_in_cooked:
			if (cooked_exists and assets_not_in_cooked):
				log('  \n\t\t\tNEW ASSETS')
				log_asset_group_by_type(assets_not_in_cooked)
			log('  \n\t\t\tEXISTING ASSETS (will be overwritten)')
			log_asset_group_by_type(assets_in_cooked, False)
		log('  \n\t\t\tASSETS TO COOK')
	else:
		log('  \n\t\t\tASSETS COOKED')
	log_asset_flat(assets_to_cook, True, True)

def pre_cook():
	global assets_to_cook, assets_in_manifests, assets_in_cooked, assets_not_in_cooked, cooked_exists
	if cooked_exists:
		with zipfile.ZipFile(cooked_path, 'r') as archive:
			for asset_id in archive.namelist():
				if (asset_id.endswith('/') or asset_id.endswith('\\')):
					continue
				found = False
				for asset in assets_in_manifests:
					if (asset_id == asset.id):
						found = True
						break
				if (not found) and (manifest_ext not in asset_id):
					assets_only_in_cooked.append(asset_id)
	assets_in_manifests = list(set(assets_in_manifests))
	assets_in_manifests.sort(key = lambda x: x.id)
	assets_in_manifests.sort(key = lambda x: x.type)
	assets_in_cooked = list(set(assets_in_cooked))
	assets_in_cooked.sort(key = lambda x: x.id)
	assets_in_cooked.sort(key = lambda x: x.type)
	assets_not_in_cooked = list(set(assets_not_in_cooked))
	assets_not_in_cooked.sort(key = lambda x: x.id)
	assets_not_in_cooked.sort(key = lambda x: x.type)
	assets_to_cook.extend(assets_in_manifests)
	assets_to_cook = list(set(assets_to_cook))
	assets_to_cook.sort(key = lambda x: x.id)
	assets_to_cook.sort(key = lambda x: x.type)

def cook():
	if cooked_exists:
		if (os.path.isfile(backup_path)):
			os.remove(backup_path)
		os.rename(cooked_path, backup_path)
	with zipfile.ZipFile(cooked_path, 'w', zipfile.ZIP_DEFLATED, True) as zipF:
		count = 0
		log('')
		for asset in assets_to_cook:
			count += 1
			zipF.write(asset.path, asset.id)
			log(('  ' + str(count) + '. ').ljust(7) + ('[' + sizeof_fmt(asset.bytes) + ']').ljust(13) + asset.id)
		log('')

def call():
	global mode
	mode = Mode.Cook
	return run()

def run():
	if not init():
		return False
	for manifest in manifests:
		load_asset_manifest(manifest)
	pre_cook()
	if (assets_not_on_fs):
		if (mode == Mode.Analyse):
			log('\n  ERROR: Invalid manifest entries / missing files! Attempt to cook will fail!')
		else:
			log('\n  ERROR: Invalid manifest entries / missing files! Cook aborted!')
		return False
	if (mode == Mode.Cook):
		cook()
		log(('  Cook Completed').ljust(20) + ': ' + cooked_path + get_sizeof_file(cooked_path))
		if (backup_path):
			log(('  Back up').ljust(20) + ': ' + backup_path + get_sizeof_file(backup_path))
	else:
		log_summary()
		if (cooked_exists):
			log(('  Analysed').ljust(20) + ': ' + cooked_path + get_sizeof_file(cooked_path))
		suffix = ' '
		if (os.path.isfile(cooked_path)):
			suffix = ' and overwrite '
		print('\n  Analysis complete. Run again with -c (--cook) to generate' + suffix + cooked_path)
	return True

if (__name__ == '__main__'):
	process_args()
	run()
