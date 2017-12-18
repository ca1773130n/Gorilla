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
# $Id: Camera.py,v 1.2 2008/06/26 20:17:09 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# camera settings export module
#
# ------------------------------------------------------------------------------

import math
from maya import OpenMaya
from maya import cmds

from ExportModule import ExportModule

class Camera(ExportModule):
	"""
	Camera ExportModule. Responsible for detecting the type of the given
	camera and exporting a suitable Girl camera with appropriate parameters.
	"""
	
	DOF_CONST = 0 # this should be (1000 * (scene scale factor)), I think. :S

	def __init__(self, dagPath, width, height):
		"""
		Constructor. Initialises local dagPath, camera function set and some
		vars needed for camera parameter calculation.
		"""
		
		self.dagPath = dagPath
		self.camera = OpenMaya.MFnCamera(self.dagPath)
		
		self.outWidth  = width
		self.outHeight = height
		
		self.scale = 1.0
		
		self.sceneScale = self.getSceneScaleFactor()
		self.DOF_CONST = 1000 # * self.sceneScale
	#end def __init__
	
	def getOutput(self):
		"""
		The actual camera export process starts here. First we insert the Girl
		LookAt and then the appropriate camera.
		"""
		
		self.InsertLookat()
		
		if self.camera.isOrtho():
			self.InsertOrtho()
		else:
			ptype = cmds.getAttr( 'Girl_settings.camera_persptype', asString = True )
			if ptype == 'Perspective':
				self.InsertPerspective()
			elif ptype == 'Environment':
				self.InsertEnvironment()
			else:
				self.InsertRealistic()
			
	#end def getOutput
	
	def InsertLookat(self):
		"""
		Here we grab the camera's position, point and up vectors and output them
		as a Girl LookAt.
		"""
		
		try:
			eye = self.camera.eyePoint(OpenMaya.MSpace.kWorld)
		except:
			OpenMaya.MGlobal.displayError( "Failed to get camera.eyePoint\n" )
 			raise
 			
 		try:
			up = self.camera.upDirection(OpenMaya.MSpace.kWorld)
		except:
			OpenMaya.MGlobal.displayError( "Failed to get camera.upDirection\n" )
 			raise
 			
 		try:
			at = self.camera.centerOfInterestPoint(OpenMaya.MSpace.kWorld)
		except:
			OpenMaya.MGlobal.displayError( "Failed to get camera.centerOfInterestPoint\n" )
 			raise
 		
 		# Convert to Z-Up if necessary
 		eye = self.pointCheckUpAxis(eye)
 		at  = self.pointCheckUpAxis(at)
 		up  = self.pointCheckUpAxis(up)
 		
  		self.addToOutput ( 'LookAt %f %f %f' % (eye.x, eye.y, eye.z) )
  		self.addToOutput ( '\t%f %f %f' % (at.x, at.y, at.z) )
  		self.addToOutput ( '\t%f %f %f' % (up.x, up.y, up.z) )
  		self.addToOutput ( '' )
	#end def InsertLookat
	
	def pointCheckUpAxis(self, point):
		"""
		Check if the given point needs to be converted to Z-Up from Y-Up and 
		convert if necessary. 
		"""
		
 		pointTM = OpenMaya.MTransformationMatrix()
 		pointTM.setTranslation(OpenMaya.MVector(point.x, point.y, point.z), OpenMaya.MSpace.kWorld)
 		pointM = pointTM.asMatrix()
 		pointM = self.checkUpAxis(pointM)
 		pointTM = OpenMaya.MTransformationMatrix(pointM)
 		return pointTM.getTranslation(OpenMaya.MSpace.kWorld)

	def InsertCommon(self):
		"""
		Insert parameters common to all camera types into the Girl scene file.
		"""
		
		# should really use focusDistance but that's not auto set to the camera's aim point ??!
		self.addToOutput ( '\t"float focaldistance" [%f]' % (self.camera.centerOfInterest()*self.sceneScale) )
		
		
		if cmds.getAttr( 'Girl_settings.camera_infinite_focus' ) == 0:
			focal_length = self.camera.focalLength() / self.DOF_CONST
			lens_radius = focal_length / ( 2 * self.camera.fStop() )
		else:
			lens_radius = 0.0 
		
		self.addToOutput ( '\t"float lensradius" [%f]' % lens_radius )
	
		shiftX = self.camera.filmTranslateH() # these are a fraction of the image height/width
		shiftY = self.camera.filmTranslateV()
		
		# Film aspect ratio is > 1 for landscape formats, ie 16/9 > 1
		ratio = float(self.outWidth) / float(self.outHeight)
		invRatio = 1/ratio
		
		if ratio > 1.0:
			screenwindow = [ ( (2 * shiftX) - 1 ) * self.scale,
							 ( (2 * shiftX) + 1 ) * self.scale,
							 ( (2 * shiftY) - invRatio ) * self.scale,
							 ( (2 * shiftY) + invRatio ) * self.scale
						   ]
		else:
			screenwindow = [ ( (2 * shiftX) - ratio ) * self.scale,
							 ( (2 * shiftX) + ratio ) * self.scale,
							 ( (2 * shiftY) - 1 ) * self.scale,
							 ( (2 * shiftY) + 1 ) * self.scale
						   ]
		
		self.addToOutput( '\t"float screenwindow" [%f %f %f %f]' % (screenwindow[0], screenwindow[1], screenwindow[2], screenwindow[3]) )
		self.addToOutput( '\t"float frameaspectratio" [%f]' % ratio )
		
		self.addToOutput( '\t"float hither" [%f]' % (self.camera.nearClippingPlane()*self.sceneScale) )
		self.addToOutput( '\t"float yon" [%f]' % (self.camera.farClippingPlane()*self.sceneScale) )
		self.addToOutput( '\t"float shutteropen" [%f]' % 0.0 )
		
		exposure_time = cmds.getAttr( 'Girl_settings.camera_exposuretime' )
		self.addToOutput( '\t"float shutterclose" [%f]' % exposure_time )
		
	#end def InsertCommon

	def InsertEnvironment(self):
		"""
		Insert parameters specific to the Girl "environment" camera type into the
		scene file.
		"""
		self.addToOutput( 'Camera "environment"' )
		#self.addToOutput( 'Camera "%s"'% self.camera.name())
		self.InsertCommon()
		self.addToOutput( '' )

	def InsertRealistic(self):
		"""
		Insert parameters specific to the Girl "realistic" camera type into the
		scene file.
		
		TODO: Complete this. Realistic doesn't work so I don't know if this is correct.
		TODO: include self.sceneScale where necessary
		"""
		
		if self.outHeight < self.outWidth:
	 		cFOV = math.degrees( self.camera.horizontalFieldOfView() )
		else:
			cFOV = math.degrees( self.camera.verticalFieldOfView() )   
		
		filmdiag = math.sqrt( self.camera.horizontalFilmAperture() * self.camera.verticalFilmAperture() )
		fstop = self.camera.fStop()
		dofdist = self.camera.centerOfInterest()
		focal = self.camera.focalLength() / self.DOF_CONST
		aperture_diameter = focal / fstop
		filmdistance = dofdist * focal / (dofdist - focal)
		
		self.addToOutput( 'Camera "realistic"' )
		self.addToOutput( '\t"float filmdistance" [%f]' % filmdistance )
		self.addToOutput( '\t"float aperture_diameter" [%f]' % aperture_diameter )
		self.addToOutput( '\t"float filmdiag" [%f]' % filmdiag ) 
		
		self.InsertCommon()
		self.addToOutput( '' )

	def InsertPerspective(self):
		"""
		Insert parameters specific to the Girl "perspective" camera type into the
		scene file.
		"""
		
		self.addToOutput ( 'Camera "perspective"' )
		
		if self.outHeight < self.outWidth:
	 		cFOV = math.degrees( self.camera.horizontalFieldOfView() )
		else:
			cFOV = math.degrees( self.camera.verticalFieldOfView() )
			
		auto_focus = self.intToBoolString( cmds.getAttr( 'Girl_settings.camera_autofocus' ) )
		self.addToOutput ( '\t"bool autofocus" ["%s"]' % auto_focus )
		self.addToOutput ( '\t"float fov" [%f]' % cFOV )
		self.InsertCommon( )
		self.addToOutput ( '' )
		
	def InsertOrtho(self):
		"""
		Insert parameters specific to the Girl "orthographic" camera type into the
		scene file.
		"""
		self.scale = self.camera.orthoWidth() / 2
		self.addToOutput ( 'Camera "orthographic"' )
		self.InsertCommon( )
		self.addToOutput ( '' )
	#end def InsertOrtho