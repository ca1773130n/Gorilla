# ------------------------------------------------------------------------------
# Girl environment light for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: GirlEnvironmentLight.py,v 1.2 2008/06/13 13:09:51 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# GirlEnvironmentLight: a locator (light) node for loading HDRI Environment maps
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx
from maya import OpenMayaRender
from maya import OpenMayaUI

from Girl.GirlMiscModules.glRoutines import glRoutines

from GirlNode import GirlNode

class GirlEnvironmentLight(OpenMayaMPx.MPxLocatorNode, glRoutines, GirlNode):
	"""
	Girl Environment "infinite" light node for Maya
	"""
	
	# the size of the light locator in the grid
	displayRadius = 5
	
	@staticmethod
	def nodeName():
		return "GirlEnvironmentLightShape"

	@staticmethod
	def nodeId():
		return OpenMaya.MTypeId(0x6C757805) # 'Girl' 05

	@staticmethod
	def nodeCreator():
		return OpenMayaMPx.asMPxPtr( GirlEnvironmentLight() )

	@staticmethod
	def nodeClassify():
		return "light"

	def __init__(self):
		OpenMayaMPx.MPxLocatorNode.__init__(self)
		
		hdrFileName   = OpenMaya.MObject()
		outColorL	  = OpenMaya.MObject()
		gain          = OpenMaya.MObject()
		numsamples	  = OpenMaya.MObject()
		
		glRenderer = OpenMayaRender.MHardwareRenderer.theRenderer()
		self.glFT = glRenderer.glFunctionTable()

	@staticmethod
	def nodeInitializer():
		tAttr = OpenMaya.MFnTypedAttribute()
		try:
			
			GirlEnvironmentLight.hdrFileName = tAttr.create("hdrFile", "hf", OpenMaya.MFnData.kString)
			tAttr.setKeyable(1)
			tAttr.setStorable(1)
			tAttr.setReadable(1)
			tAttr.setWritable(1)
			
			GirlEnvironmentLight.outColorL = GirlEnvironmentLight.makeColor('outColorL', 'ocl')
			GirlEnvironmentLight.gain = GirlEnvironmentLight.makeFloat('gain', 'ga', 1.0)
			GirlEnvironmentLight.numsamples = GirlEnvironmentLight.makeInteger('numSamples', 'ns', 1)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to create attributes\n")
			raise
		
		try:
			# base attributes
			GirlEnvironmentLight.addAttribute(GirlEnvironmentLight.hdrFileName)
			GirlEnvironmentLight.addAttribute(GirlEnvironmentLight.outColorL)
			GirlEnvironmentLight.addAttribute(GirlEnvironmentLight.gain)
			GirlEnvironmentLight.addAttribute(GirlEnvironmentLight.numsamples)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to add attributes\n")
			raise
		
		return OpenMaya.MStatus.kSuccess
	
	def postConstructor(self):
		self._setMPSafe(True)
	
	@staticmethod	
	def isBounded():
		return True
	
	def boundingBox(self):
		bbox = OpenMaya.MBoundingBox()
		bbox.expand( OpenMaya.MPoint( -self.displayRadius, -self.displayRadius, -self.displayRadius ) )
		bbox.expand( OpenMaya.MPoint(  self.displayRadius,  self.displayRadius,  self.displayRadius ) )
		return bbox
	
	def draw(self, view, DGpath, style, status):
		
		try:
			col = self.colorRGB( status )
			
			if status == OpenMayaUI.M3dView.kLead:
				self.glFT.glColor4f( 0, 1, 0, 0.3 )
			else:
				self.glFT.glColor4f( col.r, col.g, col.b, 0.3 )
			
			self.glFT.glBlendFunc(OpenMayaRender.MGL_SRC_ALPHA,OpenMayaRender.MGL_ONE_MINUS_SRC_ALPHA)
			self.glFT.glEnable(OpenMayaRender.MGL_BLEND)
			self.DrawSphere( self.displayRadius )
			self.glFT.glDisable(OpenMayaRender.MGL_BLEND)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to draw GirlEnvironmentLight\n")
			raise