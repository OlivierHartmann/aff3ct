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

		info = info[len(argtype):].strip()

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
			info     = info[3:].strip()

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

def getLongestTag(tags):
	tmp = tags.split(',')
	tag = tmp[0]
	if len(tmp) > 1:
		if len(tmp[0]) < len(tmp[1]):
			tag = tmp[1]
	return tag

def getArgReference(refHeader, refBody):
	return ".. _" + refHeader + "-" + getLongestTag(refBody).strip('-') + ":\n\n"

def toLatex(value):
	if not value.startswith("in "):
		raise RuntimeError(value)

	value = value[3:]
	text = ":math:`" + value.replace("inf", "\infty") + "`"

	return text;

def makeTableLine(col1_len, col2_len, horiz = "-", vert = "+"):
	text = vert
	for i in range(col1_len):
		text += horiz
	text += vert
	for i in range(col2_len):
		text += horiz
	text += vert + "\n"

	return text

def addSpaces(text, totalLength):
	if len(text) >= totalLength:
		return text

	for i in range(totalLength - len(text)):
		text += " "

	return text

def bubbleSort(aTagList):
    for passnum in range(len(aTagList)-1,0,-1):
        for i in range(passnum):
            if aTagList[i][0].strip('-').lower() > aTagList[i+1][0].strip('-').lower():
                temp = aTagList[i]
                aTagList[i] = aTagList[i+1]
                aTagList[i+1] = temp

def sortTags(moduleMap):
	reqList = []
	for tag in moduleMap:
		if tag == "name":
			continue
		if moduleMap[tag]["required"]:
			reqList.append([tag, moduleMap[tag]])

	stdList = []
	for tag in moduleMap:
		if tag == "name":
			continue
		if not moduleMap[tag]["required"] and moduleMap[tag]["group"] == "Standard":
			stdList.append([tag, moduleMap[tag]])

	advList = []
	for tag in moduleMap:
		if tag == "name":
			continue
		if not moduleMap[tag]["required"] and moduleMap[tag]["group"] != "Standard":
			advList.append([tag, moduleMap[tag]])


	bubbleSort(reqList)
	bubbleSort(stdList)
	bubbleSort(advList)

	tagList = reqList + stdList + advList

	return tagList


def write_module(moduleMap, path, reftag):
	file = open(path, 'w')
	indent = "   "
	required_image_link = "http://jeffnielsen.com/wp-content/uploads/2014/06/required-cropped.png"
	advanced_image_link = "https://comps.canstockphoto.com/advanced-stamp-sign-stock-illustration_csp42774668.jpg"

	text  = ".. _" + reftag + "-" + moduleMap["name"].replace(' ', '-').lower() + ":\n\n"
	text += moduleMap["name"] + "\n"

	for x in range(len(moduleMap["name"])):
		text += "-"

	text += "\n\n"

	file.write(text)


	for Arg in sortTags(moduleMap):
		tag = Arg[0]

		group    = Arg[1]["group"   ]
		argtype  = Arg[1]["argtype" ]
		limits   = Arg[1]["limits"  ]
		required = Arg[1]["required"]
		default  = Arg[1]["default" ]
		needs    = Arg[1]["needs"   ]
		excludes = Arg[1]["excludes"]
		info     = Arg[1]["info"    ]

		text  = getArgReference(reftag, tag)
		text += "``" + tag + "``\n"

		for t in range(len(tag)+4):
			text += '"'
		text +="\n\n"

		if required :
			text +=	indent + ".. image:: " + required_image_link + "\n"
			text += indent + indent + ":width:  80px" + "\n"
			text += indent + indent + ":height: 30px" + "\n"
			text += indent + indent + ":align: right" + "\n"
			text += "\n"

		if group == "Advanced" :
			text +=	indent + ".. image:: " + advanced_image_link + "\n"
			text += indent + indent + ":width:  80px" + "\n"
			text += indent + indent + ":height: 80px" + "\n"
			text += indent + indent + ":align: right" + "\n"
			text += "\n"

		if argtype != "FLAG":
			value = argtype
			if argtype == "TEXT":
				value = "text"
			elif argtype == "INT":
				value = "integer"
			elif argtype == "UINT":
				value = "positive integer"
			elif argtype == "FLOAT":
				value = "real"
			elif argtype == "PATH":
				value = "path"
			elif argtype == "FILE":
				value = "file"


			text += indent + ":type: " + argtype + "\n"


		allowed_values_table = []

		if limits != "":
			__limits = ""
			pos = limits.find("{");
			if pos != -1:
				__limits  = ":Allowed values: ";
				allowed_values_table = limits[pos+1:-1].split(',')

				for t in allowed_values_table:
					__limits += "``" + t + "`` "
			else:
				__limits = ":Range: " + toLatex(limits)

			text += indent + __limits + "\n"


		if default != "":
			text += indent + ":Default: " + default + "\n"


		if len(needs):
			text += indent + ":Needs: "
			for n in needs:
				text += "``" + n + "`` "
			text += "\n"

		if len(excludes):
			text += indent + ":Excludes: "
			for e in excludes:
				text += "``" + e + "`` "
			text += "\n"

		if argtype != "FLAG":
			exampleValue = default
			if default == "":
				if argtype == "TEXT":
					if len(allowed_values_table):
						exampleValue = allowed_values_table[0]
					else:
						exampleValue = '"TODO CHECK VALUE"'
				elif argtype == "MATLAB VECTOR STYLE":
					exampleValue = '"TODO CHECK VALUE"'
				elif argtype.startswith("TIME"):
					exampleValue = "10"
				elif argtype == "UINT" or argtype == "INT":
					exampleValue = "1"
				elif argtype == "FLOAT":
					exampleValue = "1.0"
				elif argtype == "PATH":
					exampleValue = "example/path/to/the/right/place/"
				elif argtype == "FILE":
					exampleValue = "example/path/to/the/right/file"

			text += indent + ":Examples: ``" + getLongestTag(tag) + " " + exampleValue + "``\n"

		text += "\n\n"

		text += info.replace("--", "\\\\-\\\\-") + "\n\n"


		if len(allowed_values_table):
			text += "Description of the allowed values:\n\n"

			valueHead = " Value "
			descrHead = " Description "

			longestValue = len(valueHead)
			for v in allowed_values_table:
				l = len(v) + 6
				if l > longestValue :
					longestValue = l

			descrSubstitution  = getLongestTag(tag).strip('-') + "_descr_"
			longestDescription = len(descrHead)
			for v in allowed_values_table:
				l = len(v) + len(descrSubstitution) + 4
				if l > longestDescription :
					longestDescription = l

			text += makeTableLine(longestValue, longestDescription, "-", "+")
			text +=( "|" + addSpaces(valueHead, longestValue) +
			        "|" + addSpaces(descrHead, longestDescription) +
			        "|\n")
			text += makeTableLine(longestValue, longestDescription, "=", "+")

			for v in allowed_values_table:
				text +=("|" + addSpaces(" ``" + v + "`` ", longestValue) +
				        "|" + addSpaces(" |" + descrSubstitution + v.lower() + "| ", longestDescription) +
				        "|\n")
				text += makeTableLine(longestValue, longestDescription, "-", "+")

			text += "\n"
			for v in allowed_values_table:
				text += ".. |" + descrSubstitution + v.lower() + "| replace:: TODO VALUE " + v + "\n"

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


	write_module(helpMap["global"], destPath + "global.rst", "global");
	write_module(helpMap["sim"   ], destPath + "sim.rst"   , "sim");
	write_module(helpMap["src"   ], destPath + "src.rst"   , "src");
	write_module(helpMap["crc"   ], destPath + "crc.rst"   , "crc");
	write_module(helpMap["mdm"   ], destPath + "mdm.rst"   , "mdm");
	write_module(helpMap["chn"   ], destPath + "chn.rst"   , "chn");
	write_module(helpMap["mnt"   ], destPath + "mnt.rst"   , "mnt");
	write_module(helpMap["ter"   ], destPath + "ter.rst"   , "ter");

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
