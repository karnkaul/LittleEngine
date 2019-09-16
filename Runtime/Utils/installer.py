import platform
import os
import shutil
import stat
import sys
import tarfile
import zipfile

# Parameters
game_file = 'LEDemo'

# Globals
targets = []
zip_infos = []
contents = []
os_name = ''
fatal_error = ''
install_dir = ''
ext = ''
lib_archive = ''
files_missing_msg = 'Required files missing from archive'

# Implementation
class Target:
	def __init__(self, zip_info, dest):
		self.zip_info = zip_info
		self.dest = dest

def install_path(path):
	return install_dir + '/' + path

def extract_to(source, dest):
	if (source in contents):
		with zipfile.ZipFile(game_file + '.game', 'r') as archive:
			zip_info = archive.getinfo(source)
			zip_info.filename = os.path.basename(zip_info.filename)
			targets.append(Target(zip_info, dest))
			return True
	return False

def extract_all(sources, required = False, prefix = ''):
	global fatal_error
	for source in sources:
		found = extract_to(prefix + source, install_dir)
		if required and not found:
			fatal_error = files_missing_msg
			return

def purge(path):
	if (os.path.isfile(path)):
		os.remove(path)
	elif(os.path.isdir(path)):
		shutil.rmtree(path, True)

def remove_from_install(paths):
	for p in paths:
		purge(install_path(p))

def process_archive():
	global fatal_error, contents
	with zipfile.ZipFile(game_file + '.game', 'r') as archive:
		zip_infos = archive.infolist()
		contents = archive.namelist()
		game_music_found = False
		for zip_info in zip_infos:
			if zip_info.filename.startswith('GameMusic/'):
				game_music_found = True
				targets.append(Target(zip_info, install_dir))
		if not game_music_found:
			fatal_error = files_missing_msg
		if (not 'GameAssets.cooked' in contents):
			fatal_error = files_missing_msg
		else:
			zip_info = archive.getinfo('GameAssets.cooked')
			targets.append(Target(zip_info, install_dir))

def process_archive_arch():
	global fatal_error
	if (os_name == 'Linux'):
		extract_all(['Linux/' + game_file + ext], True)
		extract_all([lib_archive, game_file + '.desktop', 'Icon.png'], False, 'Linux/')
	elif (os_name == 'Win64'):
		extract_all(['Win64/' + game_file + '.exe', 'Win64/openal32.dll'], True)
	elif (os_name == 'MacOSX'):
		extract_all(['MacOSX/' + game_file + '.app.tar.gz'], True)
	else:
		fatal_error = os_name + ' not supported!'

def install():
	print('  Installing to ' + install_dir)
	if not os.path.isdir(install_dir):
		os.makedirs(install_dir)
	# Extract all targets
	with zipfile.ZipFile(game_file + '.game', 'r') as archive:
		for target in targets:
			archive.extract(target.zip_info, target.dest)
	if (os_name == 'Linux'):
		# Remove extension from executable
		os.rename(install_path(game_file + ext), install_path(game_file))
		# Install shared libs if present
		if (os.path.isfile(install_path(lib_archive))):
			with tarfile.open(install_path(lib_archive), 'r:*') as archive:
				archive.extractall(install_dir + '/Lib/')
			remove_from_install([lib_archive])
		# Add Icon and version to .desktop and move to .local/share/applications
		desktop_file = install_path(game_file + '.desktop')
		if (os.path.isfile(desktop_file)):
			f = open(desktop_file, 'a+')
			f.write('Exec=' + install_path(game_file) + '\n')
			f.write('Icon=' + install_path('Icon.png') + '\n')
			f.close()
			home = os.path.expanduser('~')
			os.makedirs(home + '/.local/share/applications', 0o777, True)
			shutil.copy2(desktop_file, home + '/.local/share/applications/' + game_file + '.desktop')
			os.makedirs(home + '/Desktop', 0o777, True)
			shutil.move(desktop_file, home +  '/Desktop/' + game_file + '.desktop')
		# Set game_file as executable
		os.chmod(install_path(game_file), 0o751)
		dev_path='../Linux'
		if os.path.isdir(dev_path):
			# Set all files ending with ext as executable
			for exe in os.listdir(dev_path):
				if (exe.endswith(ext)):
					os.chmod(dev_path + '/' + exe, 0o751)
	elif (os_name == 'MacOSX'):
		# Extract .app from .tar.gz, also in parent dir if exists
		archive_file = game_file + '.app.tar.gz'
		with tarfile.open(install_path(archive_file)) as archive:
			archive.extractall(install_dir)
		remove_from_install([archive_file])
		dev_path = '../MacOSX'
		if os.path.isdir(dev_path):
			# Decompress other bundles
			for archive_file in os.listdir(dev_path):
				if (archive_file.endswith('tar.gz')):
					with tarfile.open(dev_path + '/' + archive_file) as archive:
						archive.extractall(dev_path)

def prepare():
	global fatal_error
	process_archive()
	process_archive_arch()
	if fatal_error:
		print(fatal_error)
		sys.exit(-1)

def init():
	global os_name, install_dir, ext, lib_archive
	os_name = platform.system()
	if (os_name == 'Windows') or ('microsoft' in platform.uname()[3].lower()):
		os_name = 'Win64'
	if (os_name == 'Darwin'):
		os_name = 'MacOSX'
	if ((os_name == 'Linux' or os_name == 'MacOSX') and sys.version_info < (3, 0)):
		print(os_name + ' installer requires Python3!\n(Try \'python3\' instead of \'python\')')
		sys.exit(-1)
	if (os_name == 'Linux'):
		arch = os.uname()[4].lower()
		is_x64 = arch.startswith('x64') or arch.startswith('amd64') or arch.startswith('x86_64')
		is_arm = arch.startswith('arm')
		if not is_x64 and not is_arm:
			print('Unsupported architecture: ' + arch)
			sys.exit(-1)
		ext = '.lxa' if is_arm else '.lx'
		lib_archive = 'Lib-ARM.tar.xz' if is_arm else 'Lib-x64.tar.xz'
	if (sys.path[0]):
			install_dir = sys.path[0]
			os.chdir(sys.path[0])
	else:
		install_dir = os.getcwd()
	if len(sys.argv) > 1:
		install_dir = os.path.join(install_dir, sys.argv[1])

def run():
	init()
	prepare()
	install()
	print('  Installation complete!')

if __name__ == '__main__':
	run()
