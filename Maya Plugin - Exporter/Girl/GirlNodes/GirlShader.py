# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: GirlShader.py,v 1.11 2008/06/22 22:16:27 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# GirlShader: a shader node for Maya which defines all available Girl materal types
# and their available parameters
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from ShaderNode					import ShaderNode

from ShaderNodes.carpaintShader			import carpaintShader
from ShaderNodes.mbrdfShader			import mbrdfShader
from ShaderNodes.glassShader			import glassShader
from ShaderNodes.matteShader			import matteShader
from ShaderNodes.mattetranslucentShader		import mattetranslucentShader
from ShaderNodes.metalShader			import metalShader
from ShaderNodes.mixShader			import mixShader
from ShaderNodes.mirrorShader			import mirrorShader
from ShaderNodes.plasticShader			import plasticShader
from ShaderNodes.roughglassShader		import roughglassShader
from ShaderNodes.shinymetalShader		import shinymetalShader
from ShaderNodes.substrateShader		import substrateShader

from ShaderNodes.arealightShader		import arealightShader

class GirlShader(OpenMayaMPx.MPxNode, ShaderNode):
	"""
	Custom Girl material shader for Maya.
	This is a single node that contains all material types, although
	the attributes for each type are split into different modules.
	(Also inluded here is AreaLight even though it's not a material)
	
	Each of these modules has two purposes:
	1. Provide attributes for Maya to use
	2. Provide a mapping of Maya attributes -> Girl parameters to be read
	   at export time. 
	"""
	
	colorTable = {}

	@staticmethod
	def nodeName():
		return "GirlShader"

	@staticmethod
	def nodeId():
		return OpenMaya.MTypeId(0x6C757801) # 'Girl' 01

	@staticmethod
	def nodeCreator():
		return OpenMayaMPx.asMPxPtr( GirlShader() )

	@staticmethod
	def nodeClassify():
		return "shader/surface"

	def __init__(self):
		OpenMayaMPx.MPxNode.__init__(self)

		# 4 generic attributes to set up the shader's internal operation
		iMaterialType		=	OpenMaya.MObject()	# enum
		bumpMap 			=	OpenMaya.MObject()	# point
		outColor			=	OpenMaya.MObject()	# color
		transparency		=   OpenMaya.MObject()	# transparency
		
		# instances of shader types
		mBrdf				= mbrdfShader()
		carpaint			= carpaintShader()
		glass				= glassShader()
		roughglass			= roughglassShader()
		matte				= matteShader()
		mattetranslucent		= mattetranslucentShader()
		metal				= metalShader()
		mix   				= mixShader()
		shinymetal			= shinymetalShader()
		mirror				= mirrorShader
		plastic 			= plasticShader()
		substrate			= substrateShader()
		
		arealight			= arealightShader()		# not technically a shader
		

	def postConstructor(self):
		self._setMPSafe( True )
		self.setExistWithoutOutConnections( True )
		self.setExistWithoutInConnections( True )
		
		self.colorTable = {
					  0: self.carpaint.kd,
					  1: self.glass.kt,
					  2: self.roughglass.kt,
					  3: self.matte.kd,
					  4: self.mattetranslucent.kt,
					  #5: self.metal.n,
					  6: self.shinymetal.ks,
					  7: self.mirror.kr,
					  8: self.plastic.kd,
					  9: self.substrate.kd,
					  10: self.arealight.L
					  }

	def compute(self, plug, block):
		if plug == self.outColor or plug.parent() == self.outColor:
			# THIS IS A VERY SIMPLE FLAT COLOUR SHADER.
			# I CAN'T GET THE LAMBERTIAN BIT TO WORK
			
			matType = block.inputValue( self.iMaterialType ).asInt()
			
			# choose the appropriate color
			if matType in self.colorTable:
				resultColor = block.inputValue( self.colorTable[matType] ).asFloatVector()
			else:
				resultColor = OpenMaya.MFloatVector(0.0, 0.0, 0.0)
			

			# set the output as a flat color
			outColorHandle = block.outputValue( self.outColor )
			outColorHandle.setMFloatVector(resultColor)
			outColorHandle.setClean()
			return OpenMaya.MStatus.kSuccess
		else:
			return OpenMaya.kUnknownParameter

	@staticmethod
	def nodeInitializer():
		nAttr		= OpenMaya.MFnNumericAttribute()
		enumAttr	= OpenMaya.MFnEnumAttribute()


		# reference instances of shader types
		GirlShader.mBrdf			= mbrdfShader()
		GirlShader.carpaint			= carpaintShader()
		GirlShader.glass 			= glassShader()
		GirlShader.roughglass			= roughglassShader()
		GirlShader.matte 			= matteShader()
		GirlShader.mattetranslucent		= mattetranslucentShader()
		GirlShader.metal 			= metalShader()
		GirlShader.mix	 			= mixShader()
		GirlShader.shinymetal			= shinymetalShader()
		GirlShader.mirror			= mirrorShader()
		GirlShader.plastic			= plasticShader()
		GirlShader.substrate 			= substrateShader()

		GirlShader.arealight			= arealightShader()

		try:
			# set up the base attributes

			# the different material types available herewithin
			GirlShader.iMaterialType = enumAttr.create("material", "mat",	13)
			enumAttr.addField( "Measured BRDF",				13)
			enumAttr.addField( "Carpaint",					0)
			enumAttr.addField( "Glass", 					1)
			enumAttr.addField( "Rough Glass",				2)
			enumAttr.addField( "Matte", 					3)
			enumAttr.addField( "Matte Translucent", 			4)
			enumAttr.addField( "Metal", 					5)
			# hmm out of sequence Enum values have no effect
			enumAttr.addField( "Mix",				   11)
			enumAttr.addField( "Null",				   12)
			enumAttr.addField( "Shiny Metal",			6)
			enumAttr.addField( "Mirror",				7)
			enumAttr.addField( "Plastic",				8)
			enumAttr.addField( "Substrate", 			9)
			enumAttr.addField( "Area Light",			 10)

			# hidden attribute to allow attaching bump maps, relevant to every material type
			GirlShader.bumpMap = nAttr.createPoint("normalCamera","n")
			GirlShader.makeInput( nAttr )
			nAttr.setHidden(1)
			nAttr.setDefault(0.0, 0.0, 0.0)

			# a special color attribute for shading purposes
			GirlShader.outColor = nAttr.createColor("outColor", "oc")
			nAttr.setKeyable(0)
			nAttr.setStorable(0)
			nAttr.setReadable(1)
			nAttr.setWritable(0)
			
			# this is set by GirlShader itself, not the user, according to the material type.
			# the actual setting of this parameter is done in the AETemplate.
			# (I'm using this to make AreaLight objects 50% transparent in the viewport)
			GirlShader.transparency = nAttr.createColor("transparency", "t")
			GirlShader.makeInput( nAttr )
			nAttr.setHidden(1)
			nAttr.setDefault(0.5, 0.5, 0.5)
			
			# init all the shader attributes
			GirlShader.mBrdf.shaderInitializer()
			GirlShader.carpaint.shaderInitializer()
			GirlShader.glass.shaderInitializer()
			GirlShader.roughglass.shaderInitializer()
			GirlShader.matte.shaderInitializer()
			GirlShader.mattetranslucent.shaderInitializer()
			GirlShader.metal.shaderInitializer()
			GirlShader.mix.shaderInitializer()
			GirlShader.shinymetal.shaderInitializer()
			GirlShader.mirror.shaderInitializer()
			GirlShader.plastic.shaderInitializer()
			GirlShader.substrate.shaderInitializer()

			GirlShader.arealight.shaderInitializer()

		except:
			OpenMaya.MGlobal.displayError("Failed to create attributes\n")
			raise

		try:
			# base attributes
			GirlShader.addAttribute(GirlShader.iMaterialType)
			GirlShader.addAttribute(GirlShader.bumpMap)
			GirlShader.addAttribute(GirlShader.outColor)
			GirlShader.addAttribute(GirlShader.transparency)

			####
			# can we defer all the below into the child/sibling classes ?

			# mbrdf
			GirlShader.addAttribute(GirlShader.mBrdf.Name)
			GirlShader.addAttribute(GirlShader.mBrdf.MaterialName)
			GirlShader.addAttribute(GirlShader.mBrdf.MaterialSubName)
			GirlShader.addAttribute(GirlShader.mBrdf.cr)
			GirlShader.addAttribute(GirlShader.mBrdf.cg)
			GirlShader.addAttribute(GirlShader.mBrdf.cb)

			# carpaint
			GirlShader.addAttribute(GirlShader.carpaint.name)
			GirlShader.addAttribute(GirlShader.carpaint.kd)
			GirlShader.addAttribute(GirlShader.carpaint.ks1)
			GirlShader.addAttribute(GirlShader.carpaint.ks2)
			GirlShader.addAttribute(GirlShader.carpaint.ks3)
			GirlShader.addAttribute(GirlShader.carpaint.r1)
			GirlShader.addAttribute(GirlShader.carpaint.r2)
			GirlShader.addAttribute(GirlShader.carpaint.r3)
			GirlShader.addAttribute(GirlShader.carpaint.m1)
			GirlShader.addAttribute(GirlShader.carpaint.m2)
			GirlShader.addAttribute(GirlShader.carpaint.m3)

			# glass
			GirlShader.addAttribute(GirlShader.glass.kr)
			GirlShader.addAttribute(GirlShader.glass.kt)
			GirlShader.addAttribute(GirlShader.glass.index)
			GirlShader.addAttribute(GirlShader.glass.cauchyb)
			GirlShader.addAttribute(GirlShader.glass.architectural)

			# roughglass
			GirlShader.addAttribute(GirlShader.roughglass.kr)
			GirlShader.addAttribute(GirlShader.roughglass.kt)
			GirlShader.addAttribute(GirlShader.roughglass.uroughness)
			GirlShader.addAttribute(GirlShader.roughglass.vroughness)
			GirlShader.addAttribute(GirlShader.roughglass.index)
			GirlShader.addAttribute(GirlShader.roughglass.cauchyb)

			# matte
			GirlShader.addAttribute(GirlShader.matte.kd)
			GirlShader.addAttribute(GirlShader.matte.sigma)

			# mattetranslucent
			GirlShader.addAttribute(GirlShader.mattetranslucent.kr)
			GirlShader.addAttribute(GirlShader.mattetranslucent.kt)
			GirlShader.addAttribute(GirlShader.mattetranslucent.sigma)

			# metal
			GirlShader.addAttribute(GirlShader.metal.name)
			#GirlShader.addAttribute(GirlShader.metal.n)
			#GirlShader.addAttribute(GirlShader.metal.k)
			GirlShader.addAttribute(GirlShader.metal.uroughness)
			GirlShader.addAttribute(GirlShader.metal.vroughness)
			GirlShader.addAttribute(GirlShader.metal.nkFile)

			# shinymetal
			GirlShader.addAttribute(GirlShader.shinymetal.kr)
			GirlShader.addAttribute(GirlShader.shinymetal.ks)
			GirlShader.addAttribute(GirlShader.shinymetal.uroughness)
			GirlShader.addAttribute(GirlShader.shinymetal.vroughness)

			# mirror
			GirlShader.addAttribute(GirlShader.mirror.kr)
			
			# mix
			GirlShader.addAttribute(GirlShader.mix.namedMaterial1)
			GirlShader.addAttribute(GirlShader.mix.namedMaterial2)
			GirlShader.addAttribute(GirlShader.mix.amount)

			# plastic
			GirlShader.addAttribute(GirlShader.plastic.kd)
			GirlShader.addAttribute(GirlShader.plastic.ks)
			GirlShader.addAttribute(GirlShader.plastic.uroughness)
			GirlShader.addAttribute(GirlShader.plastic.vroughness)

			# substrate
			GirlShader.addAttribute(GirlShader.substrate.kd)
			GirlShader.addAttribute(GirlShader.substrate.ks)
			GirlShader.addAttribute(GirlShader.substrate.uroughness)
			GirlShader.addAttribute(GirlShader.substrate.vroughness)

			# arealight
			GirlShader.addAttribute(GirlShader.arealight.L)
			GirlShader.addAttribute(GirlShader.arealight.gain)
			GirlShader.addAttribute(GirlShader.arealight.numsamples)
			

		except:
			OpenMaya.MGlobal.displayError("Failed to add attributes\n")
			raise

		try:
			GirlShader.attributeAffects(GirlShader.bumpMap,				GirlShader.outColor)
			# and these are the attributes that we use in compute
			GirlShader.attributeAffects(GirlShader.carpaint.kd,			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.glass.kt,			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.roughglass.kt, 			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.matte.kd,			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.mattetranslucent.kt,		GirlShader.outColor)
			#GirlShader.attributeAffects(GirlShader.metal.n,			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.shinymetal.kr, 			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.mirror.kr, 			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.plastic.kd,			GirlShader.outColor)
			GirlShader.attributeAffects(GirlShader.substrate.kd,			GirlShader.outColor)
			# arealight doesn't affect, values are hardcoded

		except:
			OpenMaya.MGlobal.displayError("Failed in setting attributeAffects\n")
			raise
