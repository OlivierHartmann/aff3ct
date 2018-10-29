import os
import sys
import pathlib
import argparse
import subprocess

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../refs/readers/")))
import aff3ct_trace_reader as atr




parser = argparse.ArgumentParser(prog='aff3ct-command-conversion', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('--build', action='store', dest='buildPath', type=str, default="build/", help='Build path to aff3ct.')
parser.add_argument('--dest', action='store', dest='destPath', default="doc/sphinx/source/simulator/usage/modules", type=str, help='Destination path.')


args = parser.parse_args()

# extensions = ['.txt', '.perf', '.data', '.dat']


# def getFileNames(rootPath, resultsPath, currentPath, fileNames):
# 	if os.path.isdir(currentPath):
# 		if not os.path.exists(currentPath.replace(rootPath, resultsPath)):
# 			os.makedirs(currentPath.replace(rootPath, resultsPath))

# 		files = os.listdir(currentPath)
# 		for f in files:
# 			if "~" in f:
# 				continue
# 			newCurrentPath = currentPath + "/" + f
# 			getFileNames(rootPath, resultsPath, newCurrentPath, fileNames)
# 	elif os.path.isfile(currentPath):
# 		if pathlib.Path(currentPath).suffix in extensions:
# 			if rootPath == currentPath:
# 				basename = os.path.basename(rootPath)
# 				dirname = rootPath.replace(basename, '')
# 				rootPath = dirname
# 				fileNames.append(basename)
# 			else:
# 				shortenPath = currentPath.replace(rootPath + "/", "")
# 				shortenPath = shortenPath.replace(rootPath,       "")
# 				fileNames.append(shortenPath)
# 	else:
# 		print("# (WW) The path '", currentPath, "' does not exist.")

# def is_number(s):
#     try:
#         float(s)
#         return True
#     except ValueError:
#         return False

# def dict_to_str(dict):
# 	val = "";

# 	if len(dict) == 0:
# 		return val;

# 	for key in dict:
# 		val += key + " " + dict[key] + " ";

# 	return val;

# def subcommand_to_str(dict, subcommand):
# 	args = dict_to_str(dict[subcommand]);

# 	if len(args) == 0 :
# 		return "";

# 	return subcommand + " " + args;

# def val_in_dict(dict, val, key1, key2 = ""):
# 	return (key1 in dict and dict[key1] == val) or (key2 in dict and dict[key2] == val)


# def move_option(dict_origin, key_origin, dict_dest, key_dest):
# 	if key_origin in dict_origin:
# 		dict_dest[key_dest] = dict_origin[key_origin]
# 		del dict_origin[key_origin]


# def postprocess_command(commandMap):
# 	if val_in_dict(commandMap["sim"], "POLAR", "-C", "--cde-type"):
# 		move_option(commandMap["enc"], "-K", commandMap["pct"], "-K")

# 		if "--polar-nodes" in commandMap["dec"]:
# 			if commandMap["dec"]["--polar-nodes"][0] == "{":
# 				commandMap["dec"]["--polar-nodes"] = '"' + commandMap["dec"]["--polar-nodes"] + '"'

# 	if val_in_dict(commandMap["sim"], "TURBO_PROD", "-C", "--cde-type"):
# 		move_option(commandMap["dec"], "--t", commandMap["dec"],  "--tv")
# 		move_option(commandMap["dec"], "--c", commandMap["dec"],  "--cp")
# 		move_option(commandMap["dec"], "--p", commandMap["dec"], "--pos")
# 		move_option(commandMap["dec"],  "-T", commandMap["delete"], "-T")

# 	if val_in_dict(commandMap["sim"], "UNCODED", "-C", "--cde-type"):
# 		move_option(commandMap["enc"], "-K", commandMap["src"],  "-K")

# 	if len (commandMap["pct"]) == 1 and not val_in_dict(commandMap["sim"], "TURBO_DB", "-C", "--cde-type"):
# 		move_option(commandMap["pct"], "-N", commandMap["enc"],  "-N")

# 	move_option(commandMap["dec"], "--fnc", commandMap["dec"],  "--fnc-enable")


# def convertCommand(oldCommandList):
# 	commandMap = {"sim" : {}, "crc" : {}, "src" : {}, "enc" : {}, "pct" : {}, "itl" : {}, "qnt" : {},
# 	              "mdm" : {}, "chn" : {}, "dec" : {}, "mnt" : {}, "ter" : {}, "alone" : [], "delete" : {}};
# 	tagDict    = { "p" : "sim", "m" : "sim", "M" : "sim", "s" : "sim", "C" : "sim", "R" : "sim", "E" : "sim",
# 	               "F" : "src",
# 	               "K" : "enc",
# 	               "N" : "pct",
# 	               "i" : "dec", "T" : "dec", "L" : "dec",
# 	               "e" : "mnt",
# 	               "t" : "delete"
# 	             };


# 	i = 1
# 	while i < len(oldCommandList):
# 		val = oldCommandList[i];

# 		if val[0] == '-': # it's a tag
# 			subcommand = "";
# 			command    = "";

# 			if val[1] == '-': # it's long tag
# 				subcommand = val[2:5];
# 				command    = "-" + val[5:];

# 			else: # it's small tag
# 				try:
# 					subcommand = tagDict[val[1]];
# 					command    = val;
# 				except KeyError as e:
# 					raise KeyError("In file " + fn + " : " + str(e) + "  command = " + origCommand.originalStringCommand)

# 			arg = ""
# 			if (i + 1) < len(oldCommandList):
# 				nextval = oldCommandList[i + 1]


# 				if nextval[0] == '-' and not is_number(nextval): # it may be a tag then "val" is a flag, or a negative value
# 					pass
# 				else:
# 					arg = nextval;
# 					i += 1

# 			commandMap[subcommand][command] = arg;

# 		else:
# 			commandMap["alone"].append(val);

# 		i += 1

# 	try:
# 		postprocess_command(commandMap)
# 	except KeyError as e:
# 		raise KeyError("In file " + fn + " : " + str(e) + "  command = " + origCommand.originalStringCommand)


# 	newCommand = oldCommandList[0] + " ";

# 	if len(commandMap["alone"]):
# 		printf("Ref '" + fn + "' has command with alone arguments.");
# 		print (oldCommandList)
# 		print (commandMap)
# 		print ()
# 		for v in commandMap["alone"]:
# 			newCommand += v + " "

# 	newCommand +=(subcommand_to_str(commandMap, "ter")
# 	           +  subcommand_to_str(commandMap, "crc")
# 	           +  subcommand_to_str(commandMap, "src")
# 	           +  subcommand_to_str(commandMap, "enc")
# 	           +  subcommand_to_str(commandMap, "itl")
# 	           +  subcommand_to_str(commandMap, "pct")
# 	           +  subcommand_to_str(commandMap, "mdm")
# 	           +  subcommand_to_str(commandMap, "chn")
# 	           +  subcommand_to_str(commandMap, "qnt")
# 	           +  subcommand_to_str(commandMap, "dec")
# 	           +  subcommand_to_str(commandMap, "mnt")
# 	           +  subcommand_to_str(commandMap, "sim"))

# 	return newCommand


def read_help(sim, code):

	PathOrigin = os.getcwd()
	os.chdir(args.buildPath)

	# run the tested simulator
	listArgsAFFECT = ["./bin/aff3ct", "-H", "sim", "-C", code, "--type", sim];
	try:
		processAFFECT = subprocess.Popen(listArgsAFFECT, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		(stdoutAFFECT, stderrAFFECT) = processAFFECT.communicate()
	except KeyboardInterrupt:
		os.kill(processAFFECT.pid, signal.SIGINT)
		(stdoutAFFECT, stderrAFFECT) = processAFFECT.communicate()


	returnCode = processAFFECT.returncode

	errOutput = stderrAFFECT.decode(encoding='UTF-8')
	stdOutput = stdoutAFFECT.decode(encoding='UTF-8').split("\n")

	os.chdir(PathOrigin)

	return stdOutput;

def parse_argument(command, line, group):
	splitted = line.split(' ', 1)

	tags = splitted[0]
	info = splitted[1].strip()

	command[tags] = {}


	argtype  = info.split('=')[0].split(" ")[0]
	limits   = ""
	default  = ""
	required = False
	excludes = []
	needs    = []

	if argtype in ["TEXT", "INT", "UINT", "FILE", "TIME", "MATLAB", "FLOAT"]:
		if argtype == "MATLAB":
			argtype = "MATLAB VECTOR STYLE"
		elif argtype == "TIME":
			argtype = info[:info.find("]") + 1]

		info = info[len(argtype) + 1:]

		if info.startswith("in"):
			# TODO change as regular expression
			endpos = info.find("]", 4)
			if endpos == -1:
				endpos = info.find("[", 4)
			if endpos == -1:
				endpos = info.find("}", 4)
			if endpos == -1:
				endpos = info.find("{", 4)

			if endpos == -1:
				raise RuntimeError(info)

			limits = info[:endpos+1]
			info   = info[endpos+1:]
			info   = info.strip()

		if info.startswith("="):
			default = info.split(" ")[0][1:]
			info    = info[len(default)+1:]
			info    = info.strip()

		if info.startswith("{R}"):
			required = True
			info     = info[4:]

	else: # it is a flag
		argtype = "FLAG"

	info = info.strip()

	# WARNING! Excludes and Needs check might be swapped
	if info.startswith("Excludes"):
		splitted = info.split(" ")

		i = 1
		while i < len(splitted):
			if splitted[i].startswith("-"):
				excludes.append(splitted[i])
				i += 1
			else:
				break;

		info = info.split(" ", len(excludes)+1)[len(excludes) +1]

	if info.startswith("Needs"):
		splitted = info.split(" ")

		i = 1
		while i < len(splitted):
			if splitted[i].startswith("-"):
				needs.append(splitted[i])
				i += 1
			else:
				break;

		info = info.split(" ", len(needs)+1)[len(needs) +1]


	command[tags]["group"   ] = group
	command[tags]["argtype" ] = argtype
	command[tags]["limits"  ] = limits
	command[tags]["required"] = required
	command[tags]["default" ] = default
	command[tags]["needs"   ] = needs
	command[tags]["excludes"] = excludes
	command[tags]["info"    ] = info

def help_to_map(stdOutput):
	helpMap = {}
	helpMap["global"] = {"name" : "Global parameters"}

	subcommands = False;

	i = 0
	command = "global"
	group   = ""
	name    = ""
	while i < len(stdOutput):
		l   = stdOutput[i]
		l_1 = ""

		if l == "":
			i += 1
			continue

		if (i + 1) < len(stdOutput):
			l_1 = stdOutput[i + 1].strip()

		if subcommands:
			if not l.startswith(" "):
				command = l
				helpMap[command] = {"name" : l_1}
				i += 2;
				continue;
			else:
				pass

		elif l.startswith("Subcommands"):
			subcommands = True;
			i += 1
			continue;

		l = l.strip()

		if l.startswith("-"): # its an argument

			if l_1.startswith("-"): # then following line is another argument
				l_1 = ""
			else:
				l += " " + l_1
				i += 1

			parse_argument(helpMap[command], l, group)

			i += 1
			continue;

		else: # its a group name
			group = l.strip(":")
			i += 1
			continue;


	return helpMap

def write_module(moduleMap, path):
	file = open(path, 'w')

	text = moduleMap["name"] + "\n"

	for x in range(len(moduleMap["name"])):
		text += "-"

	text += "\n\n"

	file.write(text)

	indent = "   "

	for tag in moduleMap:
		if tag == "name":
			continue

		group    = moduleMap[tag]["group"   ]
		argtype  = moduleMap[tag]["argtype" ]
		limits   = moduleMap[tag]["limits"  ]
		required = moduleMap[tag]["required"]
		default  = moduleMap[tag]["default" ]
		needs    = moduleMap[tag]["needs"   ]
		excludes = moduleMap[tag]["excludes"]
		info     = moduleMap[tag]["info"    ]

		text = "``" + tag + "``"

		if argtype != "FLAG":
			text += " + *" + argtype + "*"

		text += "\n\n"
		# text += indent + '"""\n\n'

		text += indent + ":description: " + info.replace("--", "\\\\-\\\\-") + "\n"

		if limits != "":
			__limits = limits
			pos = limits.find("{");
			if pos != -1:
				__limits  = "\n\n";
				__limits += indent + indent + ".. hlist::\n"
				__limits += indent + indent + indent + ":columns: 3\n\n"

				table = limits[pos+1:-1].split(',')

				for t in table:
					__limits += indent + indent + indent + "* `" + t + "`\n"


			text += indent + ":restrictions: " + __limits + "\n"

		text += "\n"
		# text += indent + '"""\n\n'

		text += "\n\n"

		file.write(text)

	file.close()

if __name__ == "__main__":
	######## BFER simulation
	stdOutput = read_help("BFER", "BCH");
	helpMap = help_to_map(stdOutput)


	####### write modules
	destPath = args.destPath
	if destPath[-1] != "/":
		destPath += "/"

	if not os.path.exists(destPath):
		os.makedirs(destPath)


	write_module(helpMap["global"], destPath + "global.rst");
	write_module(helpMap["sim"], destPath + "sim.rst");
	write_module(helpMap["src"], destPath + "src.rst");
	write_module(helpMap["crc"], destPath + "crc.rst");
	write_module(helpMap["mdm"], destPath + "mdm.rst");
	write_module(helpMap["chn"], destPath + "chn.rst");
	write_module(helpMap["mnt"], destPath + "mnt.rst");
	write_module(helpMap["ter"], destPath + "ter.rst");

	# write_codec(helpMap, destPath + "codec_bch");
	# write_module(helpMap["itl"], destPath + "itl");
	# write_module(helpMap["qnt"], destPath + "qnt");

	# for command in helpMap:
	# 	print (command)
	# 	for tag in helpMap[command]:
	# 		print("\t" + tag + " : ")
	# 		print(helpMap[command][tag])


	os.chdir(destPath)


	# for fn in fileNames:
	# 	# parse the reference file
	# 	originFile = atr.aff3ctTraceReader(args.path + "/" + fn);

	# 	origCommand = originFile.getCommand()
	# 	oldCommandList = origCommand.getCommandAsList()

	# 	if len(oldCommandList) == 0:
	# 		continue

	# 	newCommand = convertCommand(oldCommandList)

	# 	fSrc = open(args.path + "/" + fn,  'r')
	# 	fRes = open(args.dest + "/" + fn, 'w+')


	# 	for line in fSrc:
	# 		if line.startswith("command="):
	# 			fRes.write("command=" + newCommand + "\n")
	# 		else:
	# 			fRes.write(line)


	# 	fSrc.close()
	# 	fRes.close()
