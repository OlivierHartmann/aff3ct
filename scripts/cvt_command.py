import os
import sys
import pathlib
import argparse
import numpy as np

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "refs/readers/")))
import aff3ct_trace_reader as atr




parser = argparse.ArgumentParser(prog='aff3ct-command-conversion', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('--path', action='store', dest='path', type=str, help='Source path to the reference file(s).')
parser.add_argument('--dest', action='store', dest='dest', type=str, help='Destination path.')
parser.add_argument('--comd', action='store', dest='comd', type=str, help='A simple commmand to convert.')


args = parser.parse_args()

extensions = ['.txt', '.perf', '.data', '.dat']


def getFileNames(rootPath, resultsPath, currentPath, fileNames):
	if os.path.isdir(currentPath):
		if not os.path.exists(currentPath.replace(rootPath, resultsPath)):
			os.makedirs(currentPath.replace(rootPath, resultsPath))

		files = os.listdir(currentPath)
		for f in files:
			if "~" in f:
				continue
			newCurrentPath = currentPath + "/" + f
			getFileNames(rootPath, resultsPath, newCurrentPath, fileNames)
	elif os.path.isfile(currentPath):
		if pathlib.Path(currentPath).suffix in extensions:
			if rootPath == currentPath:
				basename = os.path.basename(rootPath)
				dirname = rootPath.replace(basename, '')
				rootPath = dirname
				fileNames.append(basename)
			else:
				shortenPath = currentPath.replace(rootPath + "/", "")
				shortenPath = shortenPath.replace(rootPath,       "")
				fileNames.append(shortenPath)
	else:
		print("# (WW) The path '", currentPath, "' does not exist.")

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def dict_to_str(dict):
	val = "";

	if len(dict) == 0:
		return val;

	for key in dict:
		val += key + " " + dict[key] + " ";

	return val;

def subcommand_to_str(dict, subcommand):
	args = dict_to_str(dict[subcommand]);

	if len(args) == 0 :
		return "";

	return subcommand + " " + args;

def val_in_dict(dict, val, key1, key2 = ""):
	return (key1 in dict and dict[key1] == val) or (key2 in dict and dict[key2] == val)


def move_option(dict_origin, key_origin, dict_dest, key_dest):
	if key_origin in dict_origin:
		dict_dest[key_dest] = dict_origin[key_origin]
		del dict_origin[key_origin]


def postprocess_command(commandMap):
	if val_in_dict(commandMap["sim"], "POLAR", "-C", "--cde-type"):
		move_option(commandMap["enc"], "-K", commandMap["pct"], "-K")

		if "--polar-nodes" in commandMap["dec"]:
			if commandMap["dec"]["--polar-nodes"][0] == "{":
				commandMap["dec"]["--polar-nodes"] = '"' + commandMap["dec"]["--polar-nodes"] + '"'

	if val_in_dict(commandMap["sim"], "TURBO_PROD", "-C", "--cde-type"):
		move_option(commandMap["dec"], "--t", commandMap["dec"],  "--tv")
		move_option(commandMap["dec"], "--c", commandMap["dec"],  "--cp")
		move_option(commandMap["dec"], "--p", commandMap["dec"], "--pos")
		move_option(commandMap["dec"],  "-T", commandMap["delete"], "-T")

	if val_in_dict(commandMap["sim"], "UNCODED", "-C", "--cde-type"):
		move_option(commandMap["enc"], "-K", commandMap["src"],  "-K")

	if len (commandMap["pct"]) == 1 and not val_in_dict(commandMap["sim"], "TURBO_DB", "-C", "--cde-type"):
		move_option(commandMap["pct"], "-N", commandMap["enc"],  "-N")

	move_option(commandMap["dec"], "--fnc", commandMap["dec"],  "--fnc-enable")


def convertCommand(oldCommandList):
	commandMap = {"sim" : {}, "crc" : {}, "src" : {}, "enc" : {}, "pct" : {}, "itl" : {}, "qnt" : {},
	              "mdm" : {}, "chn" : {}, "dec" : {}, "mnt" : {}, "ter" : {}, "alone" : [], "delete" : {}};
	tagDict    = { "p" : "sim", "m" : "sim", "M" : "sim", "s" : "sim", "C" : "sim", "R" : "sim", "E" : "sim",
	               "F" : "src",
	               "K" : "enc",
	               "N" : "pct",
	               "i" : "dec", "T" : "dec", "L" : "dec",
	               "e" : "mnt",
	               "t" : "delete"
	             };


	i = 1
	while i < len(oldCommandList):
		val = oldCommandList[i];

		if val[0] == '-': # it's a tag
			subcommand = "";
			command    = "";

			if val[1] == '-': # it's long tag
				subcommand = val[2:5];
				command    = "-" + val[5:];

			else: # it's small tag
				try:
					subcommand = tagDict[val[1]];
					command    = val;
				except KeyError as e:
					raise KeyError("In file " + fn + " : " + str(e) + "  command = " + origCommand.originalStringCommand)

			arg = ""
			if (i + 1) < len(oldCommandList):
				nextval = oldCommandList[i + 1]


				if nextval[0] == '-' and not is_number(nextval): # it may be a tag then "val" is a flag, or a negative value
					pass
				else:
					arg = nextval;
					i += 1

			commandMap[subcommand][command] = arg;

		else:
			commandMap["alone"].append(val);

		i += 1

	try:
		postprocess_command(commandMap)
	except KeyError as e:
		raise KeyError("In file " + fn + " : " + str(e) + "  command = " + origCommand.originalStringCommand)


	newCommand = oldCommandList[0] + " ";

	if len(commandMap["alone"]):
		printf("Ref '" + fn + "' has command with alone arguments.");
		print (oldCommandList)
		print (commandMap)
		print ()
		for v in commandMap["alone"]:
			newCommand += v + " "

	newCommand +=(subcommand_to_str(commandMap, "ter")
	           +  subcommand_to_str(commandMap, "crc")
	           +  subcommand_to_str(commandMap, "src")
	           +  subcommand_to_str(commandMap, "enc")
	           +  subcommand_to_str(commandMap, "itl")
	           +  subcommand_to_str(commandMap, "pct")
	           +  subcommand_to_str(commandMap, "mdm")
	           +  subcommand_to_str(commandMap, "chn")
	           +  subcommand_to_str(commandMap, "qnt")
	           +  subcommand_to_str(commandMap, "dec")
	           +  subcommand_to_str(commandMap, "mnt")
	           +  subcommand_to_str(commandMap, "sim"))

	return newCommand



if __name__ == "__main__":
	if len(args.comd) != 0:
		oldCommandList = atr.Command(args.comd).getCommandAsList()
		newCommand = convertCommand(oldCommandList)

		print(newCommand)


	elif len(args.path) != 0:
		fileNames = []
		getFileNames(args.path, args.dest, args.path, fileNames)

		for fn in fileNames:
			# parse the reference file
			originFile = atr.aff3ctTraceReader(args.path + "/" + fn);

			origCommand = originFile.getCommand()
			oldCommandList = origCommand.getCommandAsList()

			if len(oldCommandList) == 0:
				continue

			newCommand = convertCommand(oldCommandList)

			fSrc = open(args.path + "/" + fn,  'r')
			fRes = open(args.dest + "/" + fn, 'w+')


			for line in fSrc:
				if line.startswith("command="):
					fRes.write("command=" + newCommand + "\n")
				else:
					fRes.write(line)


			fSrc.close()
			fRes.close()
