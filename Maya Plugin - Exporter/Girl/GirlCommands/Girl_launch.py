# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# Based on a translation of the c++ Girlmaya exporter, in turn based on
# maya-pbrt by Mark Colbert
#
# Python translation by Doug Hammond 02/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Girllaunch.py 69 2008-05-20 16:44:36Z doug $
#
# ------------------------------------------------------------------------------
#
# Girl_launch: a command to launch the Girl executable after a successful export
#
# ------------------------------------------------------------------------------

import os
from maya import OpenMaya
from maya import OpenMayaMPx

class Girl_launch(OpenMayaMPx.MPxCommand):

	debug = False
	
	@staticmethod
	def commandName():
		return "Girl_launch"
	
	@staticmethod
	def commandCreator():
		return OpenMayaMPx.asMPxPtr( Girl_launch() )

	def __init__(self):
		OpenMayaMPx.MPxCommand.__init__(self)

	def doIt(self, args):
		
		if (args.length() < 5):
			OpenMaya.MGlobal.displayError("Girl launch must have the following arguments:\n scene file name, Girl path, Girl interface type, number of threads, process priority\n")
			return

		#// 0: scene file
		#// 1: Girl path
		#// 2: interface type (gui|console)
		#// 3: threads
		#// 4: priority

		file = args.asString(0)
		self.dprint ( file )
		
		searchpath = args.asString(1)
		self.dprint( searchpath )

		Girlinterface = args.asInt(2)
		self.dprint(Girlinterface)
		
		if Girlinterface == 0:
			GirlBinary = "girlviewer"
		else:
			GirlBinary = "GirlConsole"
			
		executable = searchpath + os.altsep + GirlBinary
		self.dprint ( executable )
		
		if os.name == "nt": executable += ".exe"
		
		if not os.path.exists(executable):
			OpenMaya.MGlobal.displayError("Girl binary not found")
			return
		
		threads = "-t %i" % args.asInt(3)
		self.dprint(threads)
		
		priority = args.asInt(4)
		self.dprint(priority)
		
		# TODO implement priority
		
		if os.path.exists(file):
			try:	
				os.spawnv(os.P_NOWAIT, executable, (executable, file, threads) ) #  could be further args in future
			except:
				OpenMaya.MGlobal.displayError( "Failed to launch process\n" )
	 			raise
		else:
			OpenMaya.MGlobal.displayError("Input file not found.")
			
	def dprint(self, str):
		if (self.debug):
			OpenMaya.MGlobal.displayInfo(str)