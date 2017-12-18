# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# Based on a translation of the c++ Girlmaya exporter, in turn based on
# Based on a translation of the c++ Girlmaya exporter, in turn based on
# maya-pbrt by Mark Colbert
#
# Python translation by Doug Hammond 02/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Light.py,v 1.2 2008/06/08 18:40:59 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# lights export module
#
# ------------------------------------------------------------------------------

import math
from maya import OpenMaya
from ExportModule import ExportModule

class Light(ExportModule):
	"""
	Light ExportModule base class. This is primarily a factory for returning
	various light types.
	"""
	
# 	def __init__(self):
# 		pass
	
	@staticmethod
	def LightFactory( fileHandle, dagPath ):
		"""
		Detect the given light type and return a corresponding light object
		"""
		
		nodeType = dagPath.node().apiType()
		
		if nodeType == OpenMaya.MFn.kSpotLight:
			#this is a spotlight
			return SpotLight( fileHandle, dagPath )
		elif nodeType == OpenMaya.MFn.kPointLight:
			#this is a pointlight
			return PointLight( fileHandle, dagPath )
		elif nodeType == OpenMaya.MFn.kDirectionalLight:
			#this is a pointlight
			return DirectionalLight( fileHandle, dagPath )
		else:
			OpenMaya.MGlobal.displayWarning("Light type %i not supported" % nodeType)
			return False
	#end def LightFactory
	
	# TODO
	#  add support for:
	#	distant
	#	goniometric
	#	projection
	
	def getOutput(self):
		"""
		Nothing to do here, child classes output light type specific syntax.
		"""
		pass
	
#end class Light

# 2008 1215 by Schumann
class DirectionalLight(Light):
	"""
	Spot light type.
	"""

	light = OpenMaya.MFnDirectionalLight()
	
	def __init__( self, fileHandle, dagPath ):
		"""
		Constructor. Sets up this object's data.
		"""
		
		self.fileHandle = fileHandle
		self.dagPath = dagPath
		self.light = OpenMaya.MFnDirectionalLight( dagPath )
	#end def __init__
		
	def getOutput(self):
		"""
		Return Girl LightSource "spot" from the given spotlight node.
		"""
		
		color = self.light.color()
		intensity = self.light.intensity()
		
		colorR = self.rgcAndClamp( color.r * intensity )
		colorG = self.rgcAndClamp( color.g * intensity )
		colorB = self.rgcAndClamp( color.b * intensity )
	
		self.addToOutput( '# Directional Light ' + self.dagPath.fullPathName() )
		self.addToOutput( 'TransformBegin' )
		self.addToOutput( self.translationMatrix( self.dagPath ) )
		self.addToOutput( '\tLightSource "distant"' )
		self.addToOutput( '\t\t"color L" [%f %f %f]' % (colorR, colorG, colorB) )
		self.addToOutput( '\t\t"point from" [0 0 0]')
		self.addToOutput( '\t\t"point to" [0 0 -1]' )
		self.addToOutput( 'TransformEnd' )
		self.addToOutput( '' )
		
		self.fileHandle.flush()
	#end def getOutput
		
#end class SpotLight

class SpotLight(Light):
	"""
	Spot light type.
	"""

	light = OpenMaya.MFnSpotLight()
	
	def __init__( self, fileHandle, dagPath ):
		"""
		Constructor. Sets up this object's data.
		"""
		
		self.fileHandle = fileHandle
		self.dagPath = dagPath
		self.light = OpenMaya.MFnSpotLight( dagPath )
	#end def __init__
		
	def getOutput(self):
		"""
		Return Girl LightSource "spot" from the given spotlight node.
		"""
		
		color = self.light.color()
		intensity = self.light.intensity()
		
		colorR = self.rgcAndClamp( color.r * intensity )
		colorG = self.rgcAndClamp( color.g * intensity )
		colorB = self.rgcAndClamp( color.b * intensity )
	
		self.addToOutput( '# Spot Light ' + self.dagPath.fullPathName() )
		self.addToOutput( 'TransformBegin' )
		self.addToOutput( self.translationMatrix( self.dagPath ) )
		self.addToOutput( '\tLightSource "spot"' )
		self.addToOutput( '\t\t"color I" [%f %f %f]' % (colorR, colorG, colorB) )
		self.addToOutput( '\t\t"point from" [0 0 0]')
		self.addToOutput( '\t\t"point to" [0 0 -1]' )
		self.addToOutput( '\t\t"float coneangle" [%f]' % ( self.light.coneAngle()*180/math.pi ) )
		self.addToOutput( '\t\t"float conedeltaangle" [%f]' % ( self.light.dropOff()*180/math.pi ) )
		self.addToOutput( 'TransformEnd' )
		self.addToOutput( '' )
		
		self.fileHandle.flush()
	#end def getOutput
		
#end class SpotLight

class PointLight(Light):
	"""
	Point light type.
	"""

	light = OpenMaya.MFnPointLight()

	def __init__( self, fileHandle, dagPath ):
		"""
		Constructor. Sets up this object's data.
		"""
		
		self.fileHandle = fileHandle
		self.dagPath = dagPath
		self.light = OpenMaya.MFnPointLight( dagPath )
	#end def __init__

	def getOutput(self):
		"""
		Return Girl LightSource "point" from the given pointlight node.
		"""
		
		color = self.light.color()
		intensity = self.light.intensity()
		
		colorR = self.rgcAndClamp( color.r * intensity )
		colorG = self.rgcAndClamp( color.g * intensity )
		colorB = self.rgcAndClamp( color.b * intensity )
		
		self.addToOutput( '# Point Light ' + self.dagPath.fullPathName() )
		self.addToOutput( 'TransformBegin' )
		self.addToOutput( self.translationMatrix( self.dagPath ) )
		self.addToOutput( '\tLightSource "point"' )
		self.addToOutput( '\t\t"color I" [%f %f %f]' % (colorR, colorG, colorB) )
		self.addToOutput( 'TransformEnd' )
		self.addToOutput( '' )
	#end def getOutput

#end class PointLight