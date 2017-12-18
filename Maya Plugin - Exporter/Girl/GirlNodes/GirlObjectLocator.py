# ------------------------------------------------------------------------------
# Girl external mesh locator node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: GirlObjectLocator.py,v 1.1 2008/06/08 12:57:40 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# GirlObjectLocator: a locator node for loading external meshes into Girl without
# having to load them into Maya first.
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx
from maya import OpenMayaRender
from maya import OpenMayaUI

from Girl.GirlMiscModules.glRoutines import glRoutines

class GirlObjectLocator(OpenMayaMPx.MPxLocatorNode, glRoutines):
	"""
	External PLY mesh file locator object
	"""

	@staticmethod
	def nodeName():
		return "GirlObjectLocator"

	@staticmethod
	def nodeId():
		return OpenMaya.MTypeId(0x6C757802) # 'Girl' 02
	
	@staticmethod
	def nodeClassify():
		return '' # NOOP for MPxLocatorNode
		
	@staticmethod
	def nodeInitializer():
		tAttr = OpenMaya.MFnTypedAttribute()
		nAttr = OpenMaya.MFnNumericAttribute()
		try:
			
			GirlObjectLocator.meshFileName = tAttr.create("meshFile", "mf", OpenMaya.MFnData.kString)
			tAttr.setKeyable(1)
			tAttr.setStorable(1)
			tAttr.setReadable(1)
			tAttr.setWritable(1)
			
			GirlObjectLocator.meshSmoothing = nAttr.create("meshSmoothing", "msm", OpenMaya.MFnNumericData.kBoolean)
			tAttr.setKeyable(1)
			tAttr.setStorable(1)
			tAttr.setReadable(1)
			tAttr.setWritable(1)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to create attributes\n")
			raise
		
		try:
			# base attributes
			GirlObjectLocator.addAttribute(GirlObjectLocator.meshFileName)
			GirlObjectLocator.addAttribute(GirlObjectLocator.meshSmoothing)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to add attributes\n")
			raise
		
		return OpenMaya.MStatus.kSuccess

	@staticmethod
	def nodeCreator():
		return GirlObjectLocator()
					
	def __init__(self):
		OpenMayaMPx.MPxLocatorNode.__init__(self)
		
		meshFileName  = OpenMaya.MObject()
		meshScale     = OpenMaya.MObject()
		meshSmoothing = OpenMaya.MObject()
		
		glRenderer = OpenMayaRender.MHardwareRenderer.theRenderer()
		self.glFT = glRenderer.glFunctionTable()
		
	def postConstructor(self):
		self._setMPSafe(True)
		
	@staticmethod	
	def isBounded():
		return True
	
	@staticmethod
	def isTransparent():
		return True
		
	def boundingBox(self):
		bbox = OpenMaya.MBoundingBox()
		bbox.expand( OpenMaya.MPoint( -0.5,  0.0, -0.5 ) )
		bbox.expand( OpenMaya.MPoint(  0.5,  0.0, -0.5 ) )
		bbox.expand( OpenMaya.MPoint(  0.5,  0.0,  0.5 ) )
		bbox.expand( OpenMaya.MPoint( -0.5,  0.0,  0.5 ) )
		bbox.expand( OpenMaya.MPoint(  0.0, -0.5,  0.0 ) )
		bbox.expand( OpenMaya.MPoint(  0.0,  0.5,  0.0 ) )
		return bbox
		
	def draw(self, view, DGpath, style, status):
		
		try:
			col = self.colorRGB( status )
			
			view.beginGL()
			self.glFT.glPushAttrib( OpenMayaRender.MGL_CURRENT_BIT )
			
			self.glFT.glEnable( OpenMayaRender.MGL_BLEND_COLOR )
			self.glFT.glBlendFunc( OpenMayaRender.MGL_SRC_ALPHA, OpenMayaRender.MGL_ONE_MINUS_SRC_ALPHA )
			
			if status == OpenMayaUI.M3dView.kLead:
				self.glFT.glColor4f( 0, 1, 0, 0.3 )
			else:
				self.glFT.glColor4f( col.r, col.g, col.b, 0.3 )
			
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(0.5,0.5,0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(-0.5,0.5,0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(0.5,-0.5,0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(-0.5,-0.5,0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
		
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(0.5,0.5,-0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(-0.5,0.5,-0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(0.5,-0.5,-0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(-0.5,-0.5,-0.5);
			self.DrawFilledCube(0.2,0.2,0.2);
			self.glFT.glPopMatrix();
			
			self.glFT.glDisable( OpenMayaRender.MGL_BLEND_COLOR )
			 
			self.glFT.glPopAttrib()
			view.endGL()
		except:
			OpenMaya.MGlobal.displayError("Failed to draw GirlObjectLocator\n")
			raise