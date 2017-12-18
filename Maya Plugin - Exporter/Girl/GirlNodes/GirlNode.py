# ------------------------------------------------------------------------------
# Girl nodes for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: GirlNode.py,v 1.7 2008/06/16 15:24:54 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl nodes and attributes for Maya ( base class )
#
# ------------------------------------------------------------------------------

import os
os.altsep = '/'
from maya import OpenMaya
from maya import cmds
from pymel import *
from Girl.GirlExportModules.ExportModule import ExportModule
import maya.cmds as cmds 

class GirlNode:
	"""
	Custom Girl node base class
	"""
	
	attributes = {}	
	outputString = str()
	
	def addToOutput(self, string):
		if not string == '': 
			self.outputString += ( string + os.linesep )
			
	def prependToOutput(self, string):
		if not string == '':
			self.outputString = string + os.linesep + self.outputString 

	# node attribute setup helper functions
	@staticmethod
	def makeInput(attr):
		attr.setKeyable(1)
		attr.setStorable(1)
		attr.setReadable(1)
		attr.setWritable(1)

	@staticmethod
	def makeOrdinary(attr):
		attr.setKeyable(1)
		attr.setStorable(1)
		attr.setReadable(0)
		attr.setWritable(1)

	@staticmethod
	def makeColor(longName, shortName, defaultR = 1.0, defaultG = 1.0, defaultB = 1.0):
		nAttr = OpenMaya.MFnNumericAttribute()
		attrOut = nAttr.createColor(longName, shortName)
		GirlNode.makeInput( nAttr )
		nAttr.setUsedAsColor(1)
		nAttr.setDefault(defaultR, defaultG, defaultB)
		return attrOut

	@staticmethod
	def makeFloat(longName, shortName, default = 0.0, input = False):
		nAttr = OpenMaya.MFnNumericAttribute()
		attrOut = nAttr.create(longName, shortName, OpenMaya.MFnNumericData.kFloat)
		if input:
			GirlNode.makeInput( nAttr )
		else:
			GirlNode.makeOrdinary( nAttr )
		nAttr.setDefault( default )
		return attrOut
	
	@staticmethod
	def makeInteger(longName, shortName, default = 0, input = False):
		nAttr = OpenMaya.MFnNumericAttribute()
		attrOut = nAttr.create(longName, shortName, OpenMaya.MFnNumericData.kInt)
		if input:
			GirlNode.makeInput( nAttr )
		else:
			GirlNode.makeOrdinary( nAttr )
		nAttr.setDefault( default )
		return attrOut
	
	@staticmethod
	def makeBoolean(longName, shortName, default = False, input = False):
		nAttr = OpenMaya.MFnNumericAttribute()
		attrOut = nAttr.create(longName, shortName, OpenMaya.MFnNumericData.kBoolean)
		if input:
			GirlNode.makeInput( nAttr )
		else:
			GirlNode.makeOrdinary( nAttr )
		# nAttr.setDefault( default )
		return attrOut


			
class NodeAttribute(ExportModule):
	"""
	Custom Girl node attribute base class
	"""
	
	GirlName = str()
	shaderNode = OpenMaya.MFnDependencyNode()
	shaderName = str()
	
	exportName = str()
	
	def __init__(self, mayaAttrName, addTo, prependTo):
		self.addToOutput = addTo
		self.prependToOutput = prependTo
		self.plugName = mayaAttrName

	
		
class ShaderColorAttribute(NodeAttribute):
	"""
	Color Attribute for Shader nodes
	"""
	
	def __init__(self, mayaAttrName):
		self.plugName = mayaAttrName		
		
	def getOutput(self, GirlName, shaderNode, shaderName, meshName, components, setCount):
		self.GirlName	= GirlName
		self.shaderNode = shaderNode
		self.shaderName = shaderName
		
		texName, inputStr = self.detectInput('color')
		
		#if self.inputFound:
		#	self.prependToOutput( inputStr )
		#	self.exportName = '%s.%s' % (texName, self.plugName)
		#else:
		self.addToOutput( 'Texture "%s.%s"' % (self.shaderName, self.GirlName) )		
			
		AttrObject = '%s.%s' % (self.shaderName, self.GirlName) 
		if cmds.connectionInfo(AttrObject, isDestination=True):
			sourceNodePath = cmds.connectionInfo(AttrObject, sourceFromDestination=True)
			[sourceNode,ooo] = sourceNodePath.split(".")
			
			"""
			Bake the shader
			"""
			print meshName
			areaSize = cmds.getAttr( '%s.as' % meshName )
			maxAreaSize = cmds.getAttr( 'Girl_settings.mas' )
			maxTexRes = cmds.getAttr( 'Girl_settings.mts' )
			texRes = math.sqrt(areaSize / maxAreaSize) * maxTexRes
			
			textureFileFormat = "png"
			fileExt = "png"
			
			if cmds.nodeType( sourceNode, api=False ) == 'bump2d' or cmds.nodeType( sourceNode, api=False ) == 'bump3d':
				sourceNodePath = cmds.connectionInfo("%s.%s" % (sourceNode, "bumpValue"), sourceFromDestination=True)
				texRes = maxTexRes
									
			ambientColor = cmds.getAttr('%s.ambientColor' % shaderName )
			[aR, aG, aB] = ambientColor[0]
			cmds.setAttr('%s.ambientColor' % shaderName, 1, 1, 1, type="double3" )
			
			sceneFilePath = cmds.getAttr( 'Girl_settings.scene_file_path' )
			sceneFileName = cmds.getAttr( 'Girl_settings.scene_filename' )
			bakedShaderPath = sceneFilePath + sceneFileName
			if( os.access(bakedShaderPath, os.F_OK) == False ): 
			    os.mkdir(bakedShaderPath)					
			bakedShaderPath = bakedShaderPath + '/' + self.shaderName
			if( os.access(bakedShaderPath, os.F_OK) == False ): 
			    os.mkdir(bakedShaderPath)			
			
			#if( cmds.nodeType(sourceNodePath, api=False) ==  'file' ):
				#[sourceNode,ooo] = sourceNodePath.split(".")
				#tmpPath = sourceNode + ".fileTextureName"
				#tmpSourcePath = cmds.getAttr( tmpPath )
				#bakedAttrFilePath = cmds.workspace( en=tmpSourcePath )
				#sourceFilePath = cmds.workspace( en=tmpSourcePath )
				#[zzz, ext] = tmpSourcePath.split(".")
				#bakedAttrFilePath = '%s/%s.%s' % (bakedShaderPath, self.GirlName, ext)
				
				#print bakedAttrFilePath
				#print sourceFilePath
				
				#os.system( "copy %s %s" % (sourceFilePath, bakedAttrFilePath) )
			#else:
			bakedAttrFilePath = '%s\\%s.%s' % (bakedShaderPath, self.GirlName, fileExt)
			selection = OpenMaya.MSelectionList()
			OpenMaya.MGlobal.getActiveSelectionList( selection )
			cmds.select( cl=True )
			
			faceName = ""
			lastIndices0 = -1
			lastIndices1 = -1
			currentIndex = -1
			pathName = meshName
			
			haveAFace = 0
			minU = 100
			maxU = -100
			minV = 100
			maxV = -100
			faceU = OpenMaya.MFloatArray()
			faceV = OpenMaya.MFloatArray()
            
			components.reset()
			while not components.isDone():
				if( lastIndices0 == -1 ):
					lastIndices0 = components.index()
					lastIndices1 = components.index()
				else:
					currentIndex = components.index()
					if( currentIndex > lastIndices1 + 1 ):
						faceName = (pathName + ".f[" + "%d" % lastIndices0 + ":" + "%d" % lastIndices1 + "] ")
						cmds.select( faceName, add=True )
						lastIndices0 = currentIndex
						lastIndices1 = currentIndex
					else:
						lastIndices1 = currentIndex
				haveAFace = 1
				
				components.getUVs( faceU, faceV )
				for i in range( faceU.length() ):
					if( faceU[i] < minU ):
						minU = faceU[i]
					if( faceU[i] > maxU ):
						maxU = faceU[i]
				for i in range( faceV.length() ):
					if( faceV[i] < minV ):
						minV = faceV[i]
					if( faceV[i] > maxV ):
						maxV = faceV[i]
					
				components.next()
			
			if( setCount == 1 ):
				minU = 0
				maxU = 1
				minV = 0
				maxV = 1
					
			if( haveAFace == 1 ):
				faceName = (pathName + ".f[" + "%d" % lastIndices0 + ":" + "%d" % lastIndices1 + "] ")
				cmds.select( faceName, add=True )
										  
			print "%f %f %f %f" % (minU, maxU, minV, maxV)
			bakedFileNode = cmds.convertSolidTx(sourceNodePath, meshName, antiAlias=1, bm=1, fts=1, sp=0, sh=0, alpha=0, doubleSided=0, uvr=[minU, maxU, minV, maxV], resolutionX=texRes, resolutionY=texRes, fileFormat=textureFileFormat, fileImageName=bakedAttrFilePath)
				
			self.addToOutput( '\t"color" "imagemap"' )
			bakedAttrFilePath = bakedAttrFilePath.replace('\\', '/')
			self.addToOutput( '\t\t"string filename" ["%s"]' % bakedAttrFilePath)
			
			# convert bump map to normal map
			if cmds.nodeType( sourceNode, api=False ) == 'bump2d' or cmds.nodeType( sourceNode, api=False ) == 'bump3d':
				bumpDepth = cmds.getAttr(sourceNode + ".bumpDepth")
				self.addToOutput( '\t\t"float bumpDepth" ["%f"]' % bumpDepth)
			
			cmds.setAttr('%s.ambientColor' % shaderName, aR, aG, aB, type="double3");
						
			#cmds.setAttr('%s.ambientColor' % shaderName, ambientColor[0], ambientColor[1], ambientColor[2], type="double3" )
		else:
			colorPlug = shaderNode.findPlug(self.plugName + "R")
			colorR = self.rgcAndClamp( colorPlug.asFloat() )
			
			colorPlug = shaderNode.findPlug(self.plugName + "G")
			colorG = self.rgcAndClamp( colorPlug.asFloat() )
			
			colorPlug = shaderNode.findPlug(self.plugName + "B")
			colorB = self.rgcAndClamp( colorPlug.asFloat() )		
			
			self.addToOutput( '\t"color" "constant"' )
			self.addToOutput( '\t\t"color value" [%f %f %f]' % (colorR, colorG, colorB) )
		
		return self.outputString
	
class ShaderFloatAttribute(NodeAttribute):
	"""
	Float Attribute for Shader nodes
	"""
	
	def __init__(self, mayaAttrName, preScale = 1, invert = False, reciprocal = False, postScale = 1):
		self.plugName = mayaAttrName
		
		self.aPreScale  = preScale
		self.aInvert	= invert
		self.aRecip     = reciprocal
		self.aPostScale = postScale
		
	def getOutput(self, GirlName, shaderNode, shaderName, meshName, components, setCount):
		self.GirlName	= GirlName
		self.shaderNode = shaderNode
		self.shaderName = shaderName
		
		texName, inputStr = self.detectInput('float')
		
		#if self.inputFound:
		#	self.prependToOutput( inputStr )
		#	self.exportName = '%s.%s' % (texName, self.plugName)
		#else:
		
		self.addToOutput( 'Texture "%s.%s"' % (self.shaderName, self.GirlName) )
		AttrObject = '%s.%s' % (self.shaderName, self.GirlName)
		if self.GirlName == 'bumpmap':
			AttrObject = '%s.normalCamera' % self.shaderName
		 
		if cmds.connectionInfo(AttrObject, isDestination=True):
			sourceNodePath = cmds.connectionInfo(AttrObject, sourceFromDestination=True)
			[sourceNode,ooo] = sourceNodePath.split(".")
			
			sourceNodePath = cmds.connectionInfo(AttrObject, sourceFromDestination=True)
			[sourceNode,ooo] = sourceNodePath.split(".")
			
			"""
			Bake the shader
			"""
			print meshName
			areaSize = cmds.getAttr( '%s.as' % meshName )
			maxAreaSize = cmds.getAttr( 'Girl_settings.mas' )
			maxTexRes = cmds.getAttr( 'Girl_settings.mts' )
			texRes = math.sqrt(areaSize / maxAreaSize) * maxTexRes
			
			textureFileFormat = "png"
			fileExt = "png"
			
			if cmds.nodeType( sourceNode, api=False ) == 'bump2d' or cmds.nodeType( sourceNode, api=False ) == 'bump3d':
				sourceNodePath = cmds.connectionInfo("%s.%s" % (sourceNode, "bumpValue"), sourceFromDestination=True)
				texRes = maxTexRes
									
			ambientColor = cmds.getAttr('%s.ambientColor' % shaderName )
			[aR, aG, aB] = ambientColor[0]
			cmds.setAttr('%s.ambientColor' % shaderName, 1, 1, 1, type="double3" )
			
			sceneFilePath = cmds.getAttr( 'Girl_settings.scene_file_path' )
			sceneFileName = cmds.getAttr( 'Girl_settings.scene_filename' )
			bakedShaderPath = sceneFilePath + sceneFileName
			if( os.access(bakedShaderPath, os.F_OK) == False ): 
			    os.mkdir(bakedShaderPath)					
			bakedShaderPath = bakedShaderPath + '/' + self.shaderName
			if( os.access(bakedShaderPath, os.F_OK) == False ): 
			    os.mkdir(bakedShaderPath)			
			
			#if( cmds.nodeType(sourceNodePath, api=False) ==  'file' ):
				#[sourceNode,ooo] = sourceNodePath.split(".")
				#tmpPath = sourceNode + ".fileTextureName"
				#tmpSourcePath = cmds.getAttr( tmpPath )
				#bakedAttrFilePath = cmds.workspace( en=tmpSourcePath )
				#sourceFilePath = cmds.workspace( en=tmpSourcePath )
				#[zzz, ext] = tmpSourcePath.split(".")
				#bakedAttrFilePath = '%s/%s.%s' % (bakedShaderPath, self.GirlName, ext)
				
				#print bakedAttrFilePath
				#print sourceFilePath
				
				#os.system( "copy %s %s" % (sourceFilePath, bakedAttrFilePath) )
			#else:
			bakedAttrFilePath = '%s\\%s.%s' % (bakedShaderPath, self.GirlName, fileExt)
			selection = OpenMaya.MSelectionList()
			OpenMaya.MGlobal.getActiveSelectionList( selection )
			cmds.select( cl=True )
			
			faceName = ""
			lastIndices0 = -1
			lastIndices1 = -1
			currentIndex = -1
			pathName = meshName
			
			haveAFace = 0
			minU = 100
			maxU = -100
			minV = 100
			maxV = -100
			faceU = OpenMaya.MFloatArray()
			faceV = OpenMaya.MFloatArray()
			
			components.reset()
			while not components.isDone():
				if( lastIndices0 == -1 ):
					lastIndices0 = components.index()
					lastIndices1 = components.index()
				else:
					currentIndex = components.index()
					if( currentIndex > lastIndices1 + 1 ):
						faceName = (pathName + ".f[" + "%d" % lastIndices0 + ":" + "%d" % lastIndices1 + "] ")
						#cmds.select( faceName, add=True )
						lastIndices0 = currentIndex
						lastIndices1 = currentIndex
					else:
						lastIndices1 = currentIndex
				haveAFace = 1
				
				components.getUVs( faceU, faceV )
				for i in range( faceU.length() ):
					if( faceU[i] < minU ):
						minU = faceU[i]
					if( faceU[i] > maxU ):
						maxU = faceU[i]
				for i in range( faceV.length() ):
					if( faceV[i] < minV ):
						minV = faceV[i]
					if( faceV[i] > maxV ):
						maxV = faceV[i]
					
				components.next()
				
			if( haveAFace == 1 ):
				faceName = (pathName + ".f[" + "%d" % lastIndices0 + ":" + "%d" % lastIndices1 + "] ")
				#cmds.select( faceName, add=True )
				
			if( setCount == 1 ):
				minU = 0
				maxU = 1
				minV = 0
				maxV = 1
													  
			print "%f %f %f %f" % (minU, maxU, minV, maxV)			
			bakedFileNode = cmds.convertSolidTx(sourceNodePath, meshName, antiAlias=1, bm=1, fts=1, sp=0, sh=0, alpha=0, doubleSided=0, uvr=[minU, maxU, minV, maxV], resolutionX=texRes, resolutionY=texRes, fileFormat=textureFileFormat, fileImageName=bakedAttrFilePath)
				
			self.addToOutput( '\t"color" "imagemap"' )
			bakedAttrFilePath = bakedAttrFilePath.replace('\\', '/')
			self.addToOutput( '\t\t"string filename" ["%s"]' % bakedAttrFilePath)
			cmds.setAttr('%s.ambientColor' % shaderName, aR, aG, aB, type="double3");
			
			if cmds.nodeType( sourceNode, api=False ) == 'bump2d' or cmds.nodeType( sourceNode, api=False ) == 'bump3d':
				bumpDepth = cmds.getAttr(sourceNode + ".bumpDepth")
				self.addToOutput( '\t\t"float bumpDepth" ["%f"]' % bumpDepth)
			
			"""
			Bake the shader
			
			print meshName
			areaSize = cmds.getAttr( '%s.as' % meshName )
			maxAreaSize = cmds.getAttr( 'Girl_settings.mas' )
			maxTexRes = cmds.getAttr( 'Girl_settings.mts' )
			texRes = math.sqrt(areaSize / maxAreaSize) * maxTexRes
			
			if cmds.nodeType( sourceNode, api=False ) == 'bump2d' or cmds.nodeType( sourceNode, api=False ) == 'bump3d':
				sourceNodePath = cmds.connectionInfo("%s.%s" % (sourceNode, "bumpValue"), sourceFromDestination=True)
				texRes = maxTexRes
			ambientColor = cmds.getAttr('%s.ambientColor' % shaderName )
			[aR, aG, aB] = ambientColor[0]
			cmds.setAttr('%s.ambientColor' % shaderName, 1, 1, 1, type="double3" )
			
			sceneFilePath = cmds.getAttr( 'Girl_settings.scene_file_path' )
			sceneFileName = cmds.getAttr( 'Girl_settings.scene_filename' )
			bakedShaderPath = sceneFilePath + sceneFileName
			if( os.access(bakedShaderPath, os.F_OK) == False ): 
			    os.mkdir(bakedShaderPath)					
			bakedShaderPath = bakedShaderPath + '\\' + self.shaderName
			if( os.access(bakedShaderPath, os.F_OK) == False ): 
			    os.mkdir(bakedShaderPath)					
			bakedAttrFilePath = '%s\\%s.png' % (bakedShaderPath, self.GirlName)
			bakedFileNode = cmds.convertSolidTx(sourceNodePath, meshName, antiAlias=1, bm=1, fts=1, sp=0, sh=0, alpha=0, doubleSided=0, componentRange=0, resolutionX=texRes, resolutionY=texRes, fileFormat="png", fileImageName=bakedAttrFilePath)
			
			self.addToOutput( '\t"color" "imagemap"' )
			bakedAttrFilePath = bakedAttrFilePath.replace('\\', '/')
			self.addToOutput( '\t\t"string filename" ["%s"]' % bakedAttrFilePath)
			
			cmds.setAttr('%s.ambientColor' % shaderName, aR, aG, aB, type="double3");
			"""			
			#cmds.setAttr('%s.ambientColor' % shaderName, ambientColor[0], ambientColor[1], ambientColor[2], type="double3" )
		else:
			floatPlug = shaderNode.findPlug(self.plugName)
			floatValue = floatPlug.asFloat() * self.aPreScale
			
			if self.aInvert:
				floatValue = 1 - floatValue
				
			if self.aRecip:
				floatValue = 1.0/floatValue
				
			floatValue *= self.aPostScale
				
			self.addToOutput( '\t"float" "constant"' )
			self.addToOutput( '\t\t"float value" [%f]' % floatValue )
		
		return self.outputString	
		
	def getFloatValue(self, shaderNode, shaderName):
		self.shaderNode = shaderNode
		self.shaderName = shaderName
		floatPlug = shaderNode.findPlug(self.plugName)
		floatValue = floatPlug.asFloat() * self.aPreScale
		
		if self.aInvert:
			floatValue = 1 - floatValue
			
		if self.aRecip:
			floatValue = 1.0/floatValue
				
		floatValue *= self.aPostScale
		
		return floatValue	

class ShaderEnumAttribute(NodeAttribute):
	"""
	Enum Attribute for Shader nodes
	"""
	
	def __init__(self, mayaAttrName, nameValues):
		self.plugName = mayaAttrName
		self.nameValues = nameValues
		
	def getOutput(self, GirlName, shaderNode, shaderName, meshName, components, setCount):
		
		ePlug = shaderNode.findPlug(self.plugName)
		eValue = ePlug.asInt()
		eValue = self.nameValues[eValue]
		
		if not eValue == "manual":
			return '\t"string %s" ["%s"]' % (GirlName, eValue)
		else:
			return ''
		
	def getStringValue(self, GirlName, shaderNode, shaderName):
		
		ePlug = shaderNode.findPlug(self.plugName)
		eValue = ePlug.asInt()
		eValue = self.nameValues[eValue]
		
		return eValue


class ShaderStringAttribute(NodeAttribute):
	"""
	String Attribute for Shader nodes
	"""
	
	def __init__(self, mayaAttrName):
		self.plugName = mayaAttrName
		
	def getOutput(self, GirlName, shaderNode, shaderName, meshName, components, setCount):
		plug = shaderNode.findPlug(self.plugName)
		self.rawValue = plug.asString()
		return '\t"string %s" ["%s"]' % (GirlName, self.rawValue)
	
class ShaderStringHideAttribute(NodeAttribute):
	"""
	String Attribute for Shader nodes
	"""
	strValue = str()

	def __init__(self, mayaAttrName, AttrValue ):
		self.plugName = mayaAttrName
		self.strValue = AttrValue
		
	def getOutput(self, GirlName, shaderNode, shaderName, meshName, components, setCount):
		return '\t"string %s" ["%s"]' % (GirlName, self.strValue)

class ShaderBoolAttribute(NodeAttribute):
	"""
	Bool Attribute for Shader nodes
	"""
	
	def __init__(self, mayaAttrName):
		self.plugName = mayaAttrName
		
	def getOutput(self, GirlName, shaderNode, shaderName, meshName, components, setCount):
		plug = shaderNode.findPlug(self.plugName)
		self.rawValue = self.intToBoolString( plug.asInt() )
		return '\t"bool %s" ["%s"]' % (GirlName, self.rawValue)
		
class TextureFloatAttribute(NodeAttribute):
	"""
	Float Attribute for Texture nodes
	"""
	
#	def __init__(self, mayaAttrName, addTo, prependTo):
#		self.addToOutput = addTo
#		self.prependToOutput = prependTo
#		self.plugName = mayaAttrName
		
	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
		cPlug = self.plugName
		self.shaderNode = textureNode
		
		texName, inputStr = self.detectInput( 'float' )
		
		if self.inputFound:
			self.prependToOutput(inputStr)
			nTextName = '%s.%s' % (texName, cPlug)
			self.addToOutput( '\t\t"texture %s" ["%s"]' % (GirlParamName, nTextName ))
		else:
			myPlug = textureNode.findPlug( cPlug  )
			value = myPlug.asFloat()
			self.addToOutput( '\t\t"float %s" [%f]' % (GirlParamName, value) )
		
		return self.outputString
	
class TextureColorAttribute(NodeAttribute):
	"""
	Color Attribute for Texture nodes
	"""
	
#	def __init__(self, mayaAttrName, addTo, prependTo):
#		self.addToOutput = addTo
#		self.prependToOutput = prependTo
#		self.plugName = mayaAttrName
		
	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
		cPlug = self.plugName
		self.shaderNode = textureNode
		
		texName, inputStr = self.detectInput( 'color' )
		
		if self.inputFound:
			self.prependToOutput(inputStr)
			nTextName = '%s.%s' % (texName, cPlug)
			self.addToOutput( '\t\t"texture %s" ["%s"]' % (cPlug, nTextName ))
		else:
			myPlugR = textureNode.findPlug( cPlug + 'R' )
			valueR = self.rgcAndClamp( myPlugR.asFloat() )
			myPlugG = textureNode.findPlug( cPlug + 'G' )
			valueG = self.rgcAndClamp( myPlugG.asFloat() )
			myPlugB = textureNode.findPlug( cPlug + 'B' )
			valueB = self.rgcAndClamp( myPlugB.asFloat() )
            
			self.addToOutput( '\t\t"color %s" [%f %f %f]' % (cPlug, valueR, valueG, valueB) )
		
		return self.outputString

class TextureBoolAttribute(NodeAttribute):
	"""
	Bool Attribute for Shader nodes
	"""
	"""
	def __init__(self, mayaAttrName):
		self.plugName = mayaAttrName
	"""	
	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
                cPlug = self.plugName
		plug = textureNode.findPlug(self.plugName)
		return '\t\t"bool %s" ["%i"]' % (cPlug, plug.asInt())

class TextureVectorAttribute(NodeAttribute):
	"""
	Vector Attribute for Texture nodes
	"""
	
#	def __init__(self, mayaAttrName, addTo, prependTo):
#		self.addToOutput = addTo
#		self.prependToOutput = prependTo
#		self.plugName = mayaAttrName
		
	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
		cPlug = self.plugName
		self.shaderNode = textureNode
		
		myPlugX = textureNode.findPlug( cPlug + 'X' )
		valueX = myPlugX.asFloat()
		myPlugY = textureNode.findPlug( cPlug + 'Y' )
		valueY = myPlugY.asFloat()
		myPlugZ = textureNode.findPlug( cPlug + 'Z' )
		valueZ = myPlugZ.asFloat()
		self.addToOutput( '\t\t"vector %s" [%f %f %f]' % (cPlug, valueX, valueY, valueZ) )
		
		return self.outputString


class TextureEnumAttribute(NodeAttribute):
	"""
	Enum Attribute for Texture nodes
	"""
	
	def __init__(self, mayaAttrName, addTo, prependTo, asString = False, nameValues = []):
		self.addToOutput = addTo
		self.prependToOutput = prependTo
		self.plugName = mayaAttrName
		self.asString = asString
		self.nameValues = nameValues
		
	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
		myPlug = textureNode.findPlug( self.plugName )
		value = myPlug.asInt()
		if self.asString:
			eValue = self.nameValues[value]
			return '\t\t"string %s" ["%s"]' % (self.plugName, eValue)
		else:
			return '\t\t"integer %s" [%i]' % (self.plugName, value)

class TextureIntegerAttribute(NodeAttribute):
	"""
	Integer Attribute for Texture nodes
	"""
	
#	def __init__(self, mayaAttrName, addTo, prependTo):
#		self.addToOutput = addTo
#		self.prependToOutput = prependTo
#		self.plugName = mayaAttrName
		
	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
		myPlug = textureNode.findPlug( self.plugName )
		value = myPlug.asInt()
		return '\t\t"integer %s" [%i]' % (self.plugName, value)
		
class TextureStringAttribute(NodeAttribute):
	"""
	String Attribute for Texture nodes
	"""
	
#	def __init__(self, mayaAttrName, addTo, prependTo):
#		self.addToOutput = addTo
#		self.prependToOutput = prependTo
#		self.plugName = mayaAttrName

	def getOutput(self, GirlParamName, textureNode, textureName, GirlName):
		myPlug = textureNode.findPlug( self.plugName )
		value = myPlug.asString()
		return '\t\t"string %s" ["%s"]' % (self.plugName, value)