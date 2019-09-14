from shutil import rmtree
import datetime
import os
import sys
import asset_cooker
import packager

os.chdir(sys.path[0])

ledemo_game_manifest = './LEDemo.game.json'
ledemo_zip_manifest = './LEDemo.zip.json'
littlengine_zip_manifest = './LittleEngine.zip.json'

version = '0.0'
cook = False

# Execution
def init():
	if (os.getcwd().endswith('Runtime')):
		os.chdir('Utils')
	error=False
	if (not os.path.isfile(ledemo_game_manifest)):
		error = True
		print('\nERROR: ' + ledemo_game_manifest + ' missing!')
	if (not os.path.isfile(ledemo_zip_manifest)):
		error = True
		print('\nERROR: ' + ledemo_zip_manifest + ' missing!')
	if (not os.path.isfile(littlengine_zip_manifest)):
		error = True
		print('\nERROR: ' + littlengine_zip_manifest + ' missing!')
	if (error):
		return False
	now = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M'))
	print('\n\t\t  APP PACKAGER\n\t\t' + now + '\n')
	return True

def call(version, cook_assets):
	fatal_error = '\n  FATAL ERROR(s)! Aborting package!\n'
	if (not init()):
		print(fatal_error)
		return False
	if (cook):
		if not asset_cooker.call():
			print(fatal_error)
			return False
	if (packager.call(ledemo_game_manifest, packager.out_path + '/Intermediate') == 0):
		if (packager.call(ledemo_zip_manifest, packager.out_path + '/Intermediate') == 0):
			if (packager.call(littlengine_zip_manifest, packager.out_path, version) == 0):
				print('\n  All packaging completed successfully\n')
				return True
			else:
				print('\n  Errors encountered\n')
	return False

def run():
	global version, cook
	for arg in sys.argv:
		if (arg.startswith('-v')):
			version = arg
		elif (arg == '-c' or arg == '--cook'):
			cook = True
	return call(version, cook)

if __name__ == '__main__':
	run()
