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
# $Id: Girl_export.py,v 1.9 2008/06/21 16:51:34 dougal2 Exp $
#
# ------------------------------------------------------------------------------

import Girl.GirlExportModules.Film			as GirlModuleFilm
import Girl.GirlExportModules.Rendersettings  as GirlModuleRendersettings
import Girl.GirlExportModules.Camera			as GirlModuleCamera
import Girl.GirlExportModules.Light			as GirlModuleLight
#import Girl.GirlExportModules.Mesh			as GirlModuleMesh
import Girl.GirlExportModules.MeshOpt			as GirlModuleMeshOpt
import Girl.GirlExportModules.Nurbs			as GirlModuleNurbs
import Girl.GirlExportModules.Subdiv			as GirlModuleSubdiv
import Girl.GirlExportModules.Volume			as GirlModuleVolume
import Girl.GirlExportModules.Material		as GirlModuleMaterial
import Girl.GirlExportModules.MiscNodes		as GirlModuleMiscNodes

import os
os.altsep = '/'
from maya import OpenMaya
from maya import OpenMayaUI
from maya import cmds

global sceneFilePath

class consoleProgress:
	cProgress = 0
	tProgress = 0
	def advanceProgress(self, int):
		self.cProgress = int
		print 'Progress: %i / %i' % (self.cProgress, self.tProgress)
	def isCancelled(self):
		return False
	def setTitle(self, string):
		print string
	def setProgressStatus(self, string):
		print string
	def endProgress(self):
		print "Done!"

class Girl_export:
	"""
	Scene iterator-controller for export process.
	This class exports the current frame according to the given parameters.
	"""

	# init export vars
	debug = False					   # displays output on stdout, no file write
	tempDagPath = OpenMaya.MDagPath()  # temp storage for dagPaths in iterators
	
		
	def doIt(self, args):
		"""
		Class entry point. Starts the frame export process.
		"""
		
		if OpenMaya.MGlobal.mayaState() == OpenMaya.MGlobal.kInteractive:
			self.mProgress = OpenMayaUI.MProgressWindow()
		else:
			self.mProgress = consoleProgress()
		
		# check args length
		if (args.length() < 5):
			OpenMaya.MGlobal.displayError("5 arguments are required for Girl_export:\nfilename, imagename, width, height, cameraname\nA sixth option debug is optional")
			return
		
		# parse args into strings
		sceneFileName = args.asString(0)
		self.dprint( "Outputfile: " + sceneFileName )
		
		imagefile = args.asString(1)
		self.dprint( "Imagefile: " + imagefile )
		
		width = args.asInt(2)
		self.dprint( "width: " + str(width) )
		
		height = args.asInt(3)
		self.dprint( "height: " + str(height) )
		
		cameraName = args.asString(4)
		self.dprint( "CameraName: " + cameraName)
		
		if (args.length == 6):
			debugMode = args.asInt(5)
			if debugMode == 1:
				self.debug = True
				self.dprint("Debug logging ON")
		
		self.log("Starting Girl export:")
		
		# init output stream/files
		
		sceneFilePathParts = sceneFileName.split( os.altsep )
		
		sceneFileName = sceneFilePathParts.pop()
		materialFileName = sceneFileName.replace(".girls", ".girlm")
		meshFileName = sceneFileName.replace(".girls", ".mesh.girlo")
		nurbsFileName = sceneFileName.replace(".girls", ".nurbs.girlo")
		subdivFileName = sceneFileName.replace(".girls", ".subdiv.girlo")
		volumeFileName = sceneFileName.replace(".girls", ".volume.girlo")
		portalsMeshFileName = sceneFileName.replace(".girls", ".mesh.portals.girlo")
		portalsNurbsFileName = sceneFileName.replace(".girls", ".nurbs.portals.girlo")
		filmFileName = sceneFileName.replace(".girls", ".Film.girlf")

		#portalsSubdivFileName = sceneFileName.replace(".girls", ".subdiv.portals.girlo")
		
		sceneFilePath = os.altsep.join(sceneFilePathParts) + os.altsep
		cmds.setAttr( 'Girl_settings.scene_file_path', sceneFilePath, type='string')
		
		if not self.debug:
			try:
				self.sceneFileHandle	= open(sceneFilePath + sceneFileName, "wb")
			except:
				OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
				raise
		
		# self.log("Files opened")
		
		
		#
		# WRITE EXTERNAL FILES
		#
		
		# POLYGON MESHES
		
		if cmds.getAttr( 'Girl_settings.scene_export_meshes' ) == 1:
			if not self.debug:
				try:		
					self.meshFileHandle = open(sceneFilePath + meshFileName, "wb")
					self.materialFileHandle = open(sceneFilePath + materialFileName, "wb")
				except:
					OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
					raise		
			# loop through meshes
			self.exportType( OpenMaya.MFn.kMesh, GirlModuleMeshOpt.MeshOpt, "Mesh", (self.meshFileHandle, self.materialFileHandle) )
			if not self.debug:
				self.meshFileHandle.close()
				self.materialFileHandle.close()
		
		# NURBS SURFACES
		
		if cmds.getAttr( 'Girl_settings.scene_export_nurbs' ) == 1:
			if not self.debug:
				try:		
					self.nurbsFileHandle = open(sceneFilePath + nurbsFileName, "wb")
					self.portalsNurbsFileHandle = open(sceneFilePath + portalsNurbsFileName, "wb")
				except:
					OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
					raise		
			# loop through NURBS surfaces
			self.exportType( OpenMaya.MFn.kNurbsSurface, GirlModuleNurbs.Nurbs, "NURBS", (self.nurbsFileHandle, self.portalsNurbsFileHandle) )
			if not self.debug:
				self.nurbsFileHandle.close()
				self.portalsNurbsFileHandle.close()

		# SUBDIVS
		
		if cmds.getAttr( 'Girl_settings.scene_export_subdivs' ) == 1:
			if not self.debug:
				try:		
					self.subdivFileHandle = open(sceneFilePath + subdivFileName, "wb")
					self.portalsSubdivFileHandle = None # open(sceneFilePath + portalsMeshFileName, "wb")
				except:
					OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
					raise		
			# loop through subdivs
			self.exportType( OpenMaya.MFn.kSubdiv, GirlModuleSubdiv.Subdiv, "Subdiv", (self.subdivFileHandle, self.portalsSubdivFileHandle) )
			if not self.debug:
				self.subdivFileHandle.close()
				#self.portalsSubdivFileHandle.close()
				
				
		# FLUID VOLUMES
			
		if cmds.getAttr( 'Girl_settings.scene_export_volumes' ) == 1:
			if not self.debug:
				try:		
					self.volumeFileHandle = open(sceneFilePath + volumeFileName, "wb")
				except:
					OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
					raise		
			# loop through Fluid volumes
			self.exportType( OpenMaya.MFn.kFluid, GirlModuleVolume.Volume, "Fluid", self.volumeFileHandle )
			if not self.debug:
				self.volumeFileHandle.close()
				
		
		
		#
		# END EXTERNAL FILES
		#
		

		# output render settings
		rs = GirlModuleRendersettings.Rendersettings( volumeFile = (sceneFilePath + volumeFileName) )
		rsOut = rs.loadModule();
		self.dprint( 'Rendersettings code: ' )
		self.dprint( rsOut )
		self.dprint( "-------------" )
		
		if not self.debug:
			rs.writeTo( self.sceneFileHandle )
			
		self.log("Render settings written")
		

                if not self.debug:
                    self.sceneFileHandle.write( 'Include "%s"' % filmFileName + os.linesep + os.linesep )

                if not self.debug:
                    try:		
                        self.filmFileHandle = open(sceneFilePath + filmFileName, "wb")
                    except:
                        OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
                        raise

                film = GirlModuleFilm.Film(width, height, imagefile)
                filmOut = film.loadModule()
                self.dprint( 'Film code: ' )
                self.dprint( filmOut )
                self.dprint( "-------------" )

                if not self.debug:
                    film.writeTo( self.filmFileHandle )

                if not self.debug:
                    self.filmFileHandle.close()

		self.log("Film settings written")


		# Output the specified camera.
		found = False
		cameraItDag = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kCamera)
		cameraPath = OpenMaya.MDagPath()
		
		while not (cameraItDag.isDone() or found):
			cameraItDag.getPath(self.tempDagPath)
			currCamName = OpenMaya.MFnDagNode(self.tempDagPath.transform()).partialPathName()
			if ((currCamName == cameraName) or (cameraName == self.tempDagPath.partialPathName())):
				found = True
				cameraPath = self.tempDagPath
			#end if
			cameraItDag.next()
		#end while
		
		if not found:
			OpenMaya.MGlobal.displayError("Could not find the camera")
			return
		else:
			self.dprint( "Found camera: " + currCamName)
		
		# obtain camera settings and write to file
		cam = GirlModuleCamera.Camera(cameraPath, width, height)
		camOut = cam.loadModule()
		self.dprint( "Camera code: " )
		self.dprint( camOut )
		self.dprint( "-------------" )
		
		if not self.debug:
			cam.writeTo( self.sceneFileHandle )
			self.sceneFileHandle.write( os.linesep + 'WorldBegin' + os.linesep + os.linesep )
		
		self.log("Camera written")
		
		
		# WRITE INCLUDES IF EXTERNAL FILES EXIST
		
		for includeFile in [materialFileName, meshFileName, nurbsFileName, subdivFileName, volumeFileName]:
			if os.path.exists(sceneFilePath + includeFile):
				self.sceneFileHandle.write( 'Include "' + includeFile + '"' + os.linesep )
			

		
		# loop though lights
		self.exportType( OpenMaya.MFn.kLight, GirlModuleLight.Light.LightFactory, "Light" )
		
		
		# find external PLY meshes via GirlObjectLocators
		# find environment light via GirlEnvironmentLight
		# find sunsky via GirlSunsky
		self.log("Finding external meshes...")
		itDn = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kDependencyNode )
		while not itDn.isDone():
			# theNode = OpenMaya.MFnDependencyNode( itDn.thisNode() )
			itDn.getPath(self.tempDagPath)
			expModule = GirlModuleMiscNodes.MiscNodes.MiscNodeFactory( self.tempDagPath, [sceneFilePath + portalsMeshFileName,sceneFilePath + portalsNurbsFileName] )
			if expModule != False:
				expOut = expModule.loadModule()
				self.dprint( "Found External Mesh: " )
				self.dprint( expOut )
				self.dprint( "------------" )
				if not self.debug:
					expModule.writeTo( self.sceneFileHandle )
			itDn.next()
		self.log("...done")
		
		

		# finish off the file
		self.log("Closing files")
		if not self.debug:
			self.sceneFileHandle.write( os.linesep + 'WorldEnd' )
 			self.sceneFileHandle.close()

 		self.log("Export complete")
 		
	#end def doIt()
	
	def doItFilmExport(self, args):
		"""
		Class entry point. Starts the frame export process.
		"""
		
		if OpenMaya.MGlobal.mayaState() == OpenMaya.MGlobal.kInteractive:
			self.mProgress = OpenMayaUI.MProgressWindow()
		else:
			self.mProgress = consoleProgress()
		
		# check args length
		if (args.length() < 4):
			OpenMaya.MGlobal.displayError("5 arguments are required for Girl_export:\nfilename, imagename, width, height, cameraname\nA sixth option debug is optional")
			return
		
		# parse args into strings
		sceneFileName = args.asString(0)
		self.dprint( "Outputfile: " + sceneFileName )
		
		imagefile = args.asString(1)
		self.dprint( "Imagefile: " + imagefile )
		
		width = args.asInt(2)
		self.dprint( "width: " + str(width) )
		
		height = args.asInt(3)
		self.dprint( "height: " + str(height) )
		
		
		if (args.length == 5):
			debugMode = args.asInt(4)
			if debugMode == 1:
				self.debug = True
				self.dprint("Debug logging ON")
		
		self.log("Starting Girl Film export:")
		
		# init output stream/files
		
		sceneFilePathParts = sceneFileName.split( os.altsep )
		
		sceneFileName = sceneFilePathParts.pop()
		filmFileName = sceneFileName.replace(".girls", ".Film.girlf")
		
		sceneFilePath = os.altsep.join(sceneFilePathParts) + os.altsep

                if not self.debug:
                    try:		
                        self.filmFileHandle = open(sceneFilePath + filmFileName, "wb")
                    except:
                        OpenMaya.MGlobal.displayError( "Failed to open files for writing\n" )
                        raise

                film = GirlModuleFilm.Film(width, height, imagefile)
                filmOut = film.loadModule()
                self.dprint( 'Film code: ' )
                self.dprint( filmOut )
                self.dprint( "-------------" )

                if not self.debug:
                    film.writeTo( self.filmFileHandle )

                if not self.debug:
                    self.filmFileHandle.close()

 		self.log("Export complete")
 		
	#end def doIt()

	def exportType(self, objType, objModule, objName, theFileHandle = "_undefined"):
		"""
		Here we iterate over the specified object type, calling the specified
		export module to handle it, and do the output :)
		Shame we need a different iterator for materials, and some other objects.
		"""
		
		if theFileHandle == "_undefined":
			theFileHandle = self.sceneFileHandle
		self.log("Exporting " + objName + " objects...")
		itDag = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst, objType )
		#self.mComputation.beginComputation()
		while not itDag.isDone():
			#if self.mComputation.isInterruptRequested(): break
			if self.mProgress.isCancelled(): break
			itDag.getPath(self.tempDagPath)
			visTest = OpenMaya.MFnDagNode(self.tempDagPath)
			if self.isVisible(visTest):
				if objName == "Mesh":
					expModule = objModule(theFileHandle, self.tempDagPath, itDag.fullPathName())
				else:
				    expModule = objModule(theFileHandle, self.tempDagPath)				
				if expModule != False:
					expOut = expModule.loadModule()
					self.dprint( "Found "+objName+": " )
					self.dprint( expOut )
					self.dprint( "------------" )
					#if not self.debug:
					#	expModule.writeTo( theFileHandle )
			itDag.next()
		self.log("...done")
	
	def log(self, string):
		"""
		Update the progress window with info about what the process is doing.
		"""
		
		self.mProgress.setProgressStatus(string)

	def isVisible(self, fnDag):
		"""
		Detect if the given fnDag is visible. (Also checks fnDag's parents.)
		"""
		visible = True
		
		if fnDag.isIntermediateObject():
			visible = False
		
		try:
			visPlug = fnDag.findPlug("visibility")
			visible = visPlug.asBool()
		except:
			OpenMaya.MGlobal.displayError("MPlug.asBool")
			visible = False
			
		try:
			dOPlug = fnDag.findPlug("drawOverride")
			
			# ignore object is in template or reference layer
			normalDisplayPlug = dOPlug.child(0)
			if normalDisplayPlug.asInt() != 0:
				visible = False
			
			# ignore object if later is not visible
			layerVisiblePlug = dOPlug.child(6)
			if layerVisiblePlug.asInt() == 0:
				visible = False
		except:
			pass # this is an optional override, so forget about it if it doesn't exist

		parentCount = fnDag.parentCount()
		if parentCount > 0 and visible:
			visible = self.isVisible( OpenMaya.MFnDagNode(fnDag.parent(0)) )
			
		return visible
	# end def isVisible()
	
	def dprint(self, string):
		if (self.debug):
			self.log('DEBUG: ' + str(string) )
	# end def dprint()