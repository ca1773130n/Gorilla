# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: MiscNodes.py,v 1.6 2008/06/16 21:34:07 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# miscellaneous export module
#
# ------------------------------------------------------------------------------

import os
os.altsep = '/'
from maya import OpenMaya

from ExportModule import ExportModule

from Girl.GirlNodes.GirlObjectLocator import GirlObjectLocator
from Girl.GirlNodes.GirlEnvironmentLight import GirlEnvironmentLight
from Girl.GirlNodes.GirlSunsky import GirlSunsky
from Girl.GirlMiscModules.FileCollector import FileCollector

class MiscNodes(ExportModule):
    """
    ExportModule for miscellaneous custom nodes
    """
    
    @staticmethod
    def MiscNodeFactory( dagPath, portalsFilesNames = [] ):
        """
        Determine the type of object and return it's Loader()
        """
        
        dpNode = OpenMaya.MFnDependencyNode( dagPath.node() )
        nodeType = dpNode.typeName()
        
        if   nodeType == GirlObjectLocator.nodeName():
            return GirlObjectLocatorLoader( dagPath, dpNode )
        elif nodeType == GirlEnvironmentLight.nodeName():
            return GirlEnvironmentLightLoader( dagPath, dpNode, portalsFilesNames )
        elif nodeType == GirlSunsky.nodeName():
            return GirlSunskyLoader( dagPath, dpNode, portalsFilesNames )
        else:
            return False

class GirlSunskyLoader(ExportModule):
    """
    Constructs Girl sunsky syntax given a GirlSunsky object
    """
    
    def __init__(self, dagPath, dpNode, portalsFilesNames):
        """
        Set up the objects we're dealing with
        """
        
        self.dagPath = dagPath
        self.dpNode = dpNode
        self.portalsFiles = portalsFilesNames
        
    def getOutput(self):
        """
        Build the output
        """
        
        self.addToOutput( '# Sunsky light loaded by GirlSunsky object: ' )
        self.addToOutput( 'TransformBegin' )
        
        matrix =  self.dagPath.inclusiveMatrix()
        matrix = self.checkUpAxis(matrix)
        imatrix = matrix.inverse()
        dirX = imatrix(0,2)
        dirY = imatrix(1,2)
        dirZ = imatrix(2,2)
        # thanks Girlblend, nice and efficient
        
        sPlug = self.dpNode.findPlug("nsamples")
        nsamples = sPlug.asInt()
        
        sPlug = self.dpNode.findPlug("gain")
        gain = sPlug.asFloat()
        
        sPlug = self.dpNode.findPlug("turbidity")
        turbidity = sPlug.asFloat()
        
        sPlug = self.dpNode.findPlug("relsize")
        relsize = sPlug.asFloat()
        
        self.addToOutput( '\tLightSource "sunsky"' )
        self.addToOutput( '\t\t"vector sundir" [%f %f %f]' % (dirX, dirY, dirZ) )
        self.addToOutput( '\t\t"integer nsamples" [%i]' % nsamples )
        self.addToOutput( '\t\t"float gain" [%f]' % gain )
        self.addToOutput( '\t\t"float turbidity" [%f]' % turbidity )
        self.addToOutput( '\t\t"float relsize" [%f]' % relsize )
        for portalsFile in self.portalsFiles:
            if os.path.exists(portalsFile):
		offsetPath = portalsFile.split( os.altsep )
		offsetPortalsFile = offsetPath.pop()
                self.addToOutput( '\t\tInclude "%s"' % offsetPortalsFile )
        self.addToOutput( 'TransformEnd' )
        self.addToOutput( '' )
        
class GirlEnvironmentLightLoader(ExportModule):
    """7
    Constructs Girl infinite LightSource syntax given a GirlEnvironmentLight
    """
    
    def __init__(self, dagPath, dpNode, portalsFilesNames):
        """
        Set up the objects we're dealing with
        """
        
        self.dagPath = dagPath
        self.dpNode = dpNode
        self.portalsFiles = portalsFilesNames
        
    def getOutput(self):
        """
        Build the output
        """
        
        self.addToOutput( '# Infinite light loaded by GirlEnvironment Light object: ' )
        self.addToOutput( 'TransformBegin' )
        self.addToOutput( self.translationMatrix( self.dagPath ) )
        
        # Add local scaling
        scaleXPlug = self.dpNode.findPlug("localScaleX")
        scaleYPlug = self.dpNode.findPlug("localScaleY")
        scaleZPlug = self.dpNode.findPlug("localScaleZ")
        lScaleX = scaleXPlug.asFloat()
        lScaleY = scaleYPlug.asFloat()
        lScaleZ = scaleZPlug.asFloat()
        self.addToOutput( '\tScale %f %f %f' % ( lScaleX, lScaleY, lScaleZ ) )
        
        fileNamePlug = self.dpNode.findPlug("hdrFile")
        hdrFileName = fileNamePlug.asString()
        
        lPlug = self.dpNode.findPlug("outColorLR")
        lColorR = self.rgcAndClamp( lPlug.asFloat() )
        lPlug = self.dpNode.findPlug("outColorLG")
        lColorG = self.rgcAndClamp( lPlug.asFloat() )
        lPlug = self.dpNode.findPlug("outColorLB")
        lColorB = self.rgcAndClamp( lPlug.asFloat() )
        
        gainPlug = self.dpNode.findPlug("gain")
        gain = gainPlug.asFloat()
        
        samplesPlug = self.dpNode.findPlug("numSamples")
        numSamples = samplesPlug.asInt()
        
        self.addToOutput( '\tLightSource "infinite"' )
        if not hdrFileName == "":
            self.addToOutput( '\t\t"string mapname" ["%s"]' % FileCollector.collectHDRI( hdrFileName ) )
        self.addToOutput( '\t\t"color L" [%f %f %f]' % (lColorR, lColorG, lColorB) )
        self.addToOutput( '\t\t"float gain" [%f]' % gain )
        self.addToOutput( '\t\t"integer nsamples" [%i]' % numSamples )
        for portalsFile in self.portalsFiles:
            if os.path.exists(portalsFile):
		offsetPath = portalsFile.split( os.altsep )
		offsetPortalsFile = offsetPath.pop()
                self.addToOutput( '\t\tInclude "%s"' % offsetPortalsFile )
        self.addToOutput( 'TransformEnd' )
        self.addToOutput( '' )
       
class GirlObjectLocatorLoader(ExportModule):
    """
    Constructs plymesh Shape syntax given a GirlObjectLocator
    """
    
    def __init__(self, dagPath, dpNode):
        """
        Set up the objects we're dealing with
        """
        
        self.dagPath = dagPath
        self.dpNode = dpNode
    
    def getOutput(self):
        """
        Build the output
        """
        
        self.addToOutput( '# External mesh loaded by GirlObjectLocator object: ' )
        self.addToOutput( 'AttributeBegin' )
        
        self.addToOutput( self.translationMatrix( self.dagPath ) )
        
        # Add in local transformations to adjust for PLY file geometry location + size offset
        translateXPlug = self.dpNode.findPlug("localPositionX")
        translateYPlug = self.dpNode.findPlug("localPositionY")
        translateZPlug = self.dpNode.findPlug("localPositionZ")
        lPosX = translateXPlug.asFloat()
        lPosY = translateYPlug.asFloat()
        lPosZ = translateZPlug.asFloat()
        
        self.addToOutput( '\tTranslate %f %f %f' % ( lPosX, lPosY, lPosZ ) )
        
        scaleXPlug = self.dpNode.findPlug("localScaleX")
        scaleYPlug = self.dpNode.findPlug("localScaleY")
        scaleZPlug = self.dpNode.findPlug("localScaleZ")
        lScaleX = scaleXPlug.asFloat()
        lScaleY = scaleYPlug.asFloat()
        lScaleZ = scaleZPlug.asFloat()
        
        self.addToOutput( '\tScale %f %f %f' % ( lScaleX, lScaleY, lScaleZ ) )
        
        # self.addToOutput( self.findShader(0, 0) )
        
        fileNamePlug = self.dpNode.findPlug("meshFile")
        meshFileName = fileNamePlug.asString()
        
        saveFolderorg = cmds.getAttr( 'Girl_settings.scene_path' )
        saveFolderorg = saveFolderorg + os.altsep

        saveFolder = saveFolderorg.replace('\\', '/')
        saveFolder = saveFolder.replace('//', '/')
        saveFolder = saveFolder.replace('/Girl/', '/')

        offsetMeshFileName = meshFileName.replace(saveFolder, '../../')
        offsetMeshFileName = offsetMeshFileName.replace('../../Girl/', '../')

        smoothingPlug = self.dpNode.findPlug("meshSmoothing")
        smoothingInt = smoothingPlug.asInt()
        if smoothingInt == 1:
            smoothingString = "true"
        else:
            smoothingString = "false"
        
        self.addToOutput( '\tShape "plymesh"' )


        self.addToOutput( '\t\t"string filename" ["%s"]' % offsetMeshFileName )
        self.addToOutput( '\t\t"bool smooth" ["%s"]' % smoothingString )
        self.addToOutput( 'AttributeEnd' )
        self.addToOutput( '' )
        