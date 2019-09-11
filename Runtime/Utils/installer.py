import platform
import os
import shutil
import stat
import sys
import tarfile
import zipfile

game_file = 'LEDemo'

os_name = platform.system()
if ("microsoft" in platform.uname()[3].lower()):
	os_name = 'Windows'
if (os_name == 'Darwin'):
	os_name = 'MacOSX'

if ((os_name == 'Linux' or os_name == 'MacOSX') and sys.version_info < (3, 0)):
	print(os_name + ' installer requires Python3!\n(Try \'python3\' instead of \'python\')')
	sys.exit(-1)

class Target:
	def __init__(self, zip_info, dest):
		self.zip_info = zip_info
		self.dest = dest

targets = []
fatal_error = ''
install_dir = os.getcwd()
if len(sys.argv) > 1:
	install_dir = os.path.join(install_dir, sys.argv[1])

files_missing_msg = 'Required files missing from archive'

zip_infos = []
contents = []

def extract_to(source, dest):
	if (source in contents):
		zip_info = archive.getinfo(source)
		zip_info.filename = os.path.basename(zip_info.filename)
		targets.append(Target(zip_info, dest))
		return True
	return False

def purge(path):
	if (os.path.isfile(path)):
		os.remove(install_dir + '/' + path)
	elif(os.path.isdir(path)):
		shutil.rmtree(path, True)

def remove_from_install(paths):
	global install_dir
	for p in paths:
		purge(p)

def makedirs_in_install(dirs):
	global install_dir
	for d in dirs:
		os.makedirs(install_dir + '/' + d, 0o777, True)

def move_in_install(file_from, dir_to):
	global install_dir
	to = install_dir + '/' + dir_to + '/' + file_from
	purge(to)
	os.rename(install_dir + '/' + file_from, to)

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
		
if (os_name == 'Linux'):
	platform = os.uname()[4].lower()
	is_x64 = platform.startswith('x64') or platform.startswith('amd64') or platform.startswith('x86_64')
	if (not is_x64):
		print('Unsupported architecture: ' + platform)
		sys.exit(-1)
	else:
		if (not extract_to('Linux/' + game_file, install_dir)):
			fatal_error = files_missing_msg
		if (not extract_to('Linux/Lib.tar.xz', install_dir)):
			fatal_error = files_missing_msg
	extract_to('Linux/' + game_file + '.desktop', install_dir)
	extract_to('Linux/Icon.png', install_dir)

elif (os_name == 'Windows'):
	if (not extract_to('Win64/' + game_file + '.exe', install_dir)):
		fatal_error = files_missing_msg
	if (not extract_to('Win64/openal32.dll', install_dir)):
		fatal_error = files_missing_msg

elif (os_name == 'MacOSX'):
	if (not extract_to('OSX/' + game_file, install_dir)):
		fatal_error = files_missing_msg
	if (not extract_to('OSX/Lib.tar.gz', install_dir)):
		fatal_error = files_missing_msg
	if (not extract_to('OSX/Frameworks.tar.gz', install_dir)):
		fatal_error = files_missing_msg
	extract_to('OSX/Icon.icns', install_dir)
	extract_to('OSX/Info.plist', install_dir)

else:
	fatal_error = os_name + ' not supported!'

if fatal_error:
	print(fatal_error)
	sys.exit(-1)

if not os.path.isdir(install_dir):
	os.mkdir(install_dir)
print('  Installing to ' + install_dir)

# Extract all targets
with zipfile.ZipFile(game_file + '.game', 'r') as archive:
	for target in targets:
		archive.extract(target.zip_info, target.dest)

if (os_name == 'Linux'):
	with tarfile.open(install_dir + '/Lib.tar.xz', 'r:*') as archive:
		archive.extractall(install_dir + '/Lib/')
	remove_from_install(['Lib.tar.xz'])
	desktop_file = install_dir + '/' + game_file + '.desktop'
	if (os.path.isfile(desktop_file)):
		f = open(desktop_file, 'a+')
		f.write('Exec=' + install_dir + '/' + game_file + '\n')
		f.write('Icon=' + install_dir + '/Icon.png\n')
		home = os.path.expanduser('~')
		os.makedirs(home + '/.local/share/applications', 0o777, True)
		os.rename(desktop_file, os.path.join(home, '.local/share/applications/' + game_file + '.desktop'))
	os.chmod(install_dir + '/' + game_file, 0o751)

if (os_name == 'MacOSX'):
	with tarfile.open(install_dir + '/Lib.tar.gz', 'r:*') as archive:
		archive.extractall(install_dir + '/Lib')
	os.remove(install_dir + '/Lib.tar.gz')
	with tarfile.open(install_dir + '/Frameworks.tar.gz', 'r:*') as archive:
		archive.extractall(install_dir + '/Frameworks/')
	remove_from_install(['Lib.tar.gz, Frameworks.tar.gz'])
	os.chmod(install_dir + '/' + game_file, 0o751)
	plist = install_dir + '/Info.plist'
	icon = install_dir + '/Icon.icns'
	if (os.path.isfile(plist)):
		shutil.rmtree(install_dir + '/' + game_file + '_', True)
		makedirs_in_install([game_file + '_/Contents/MacOS', game_file + '_/Contents/Resources'])
		move_in_install(game_file, game_file + '_/Contents/MacOS')
		move_in_install('Lib', game_file + '_/Contents/MacOS')
		move_in_install('Frameworks', game_file + '_/Contents/MacOS')
		move_in_install('Info.plist', game_file + '_/Contents')
		if (os.path.isfile(icon)):
			move_in_install('Icon.icns', game_file + '_/Contents/Resources')
		shutil.rmtree(install_dir + '/' + game_file + '.app', True)
		os.rename(install_dir + '/' + game_file + '_', install_dir + '/' + game_file + '.app')
print('  Installation complete!')