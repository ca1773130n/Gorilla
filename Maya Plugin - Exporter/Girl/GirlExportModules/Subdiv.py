# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Subdiv.py,v 1.3 2008/06/21 16:51:34 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# subdiv object exporter
#
# ------------------------------------------------------------------------------

import time, os
from maya import cmds
from maya import OpenMaya

from ExportModule import ExportModule

class Subdiv(ExportModule):
    
    dagPath = OpenMaya.MDagPath()
    fShape  = OpenMaya.MFnSubd()
    
    def __init__(self, fileHandles, dagPath):
        
        self.subdivHandle, self.portalsHandle = fileHandles
        
        self.fileHandle = self.subdivHandle
        
        self.fShape = OpenMaya.MFnSubd(dagPath)
        self.dagPath = dagPath
        
        
    def resetLists(self):
        self.vertArray = {}
        self.localIndex = {}
        self.indArray = {}
        self.normArray = {}
	self.uvArray = {}
	self.i=0
        self.j=0
	self.hasUV = 0
        
    def getOutput(self):
        
        self.resetLists()
    	nSubDLevels = 1
	nSubDLevels = cmds.getAttr( 'Girl_settings.scene_export_subdivs_levels' )

        THEHIGHESTLEVEL = self.fShape.levelMaxCurrent()
	THELEVEL = 0
    
        #for THELEVEL in range(THEHIGHESTLEVEL, 0, -1):
	if THELEVEL == 0 :
	    itSV = OpenMaya.MItSubdVertex(self.dagPath.node())
	    itSV.setLevel( THELEVEL )
	    
	    while not itSV.isDone():
		cInd = itSV.index()

		if self.fShape.vertexIsValid(cInd):
		    vP = OpenMaya.MPoint()
		    self.fShape.vertexPositionGet(cInd, vP)
		    self.vertArray[self.i] = vP
		    
		    vN = OpenMaya.MVector()
		    self.fShape.vertexNormal(cInd, vN)
		    self.normArray[self.i] = vN
		    
		    vUV = OpenMaya.MPoint()
		    self.uvArray[self.i] = vUV

		    self.localIndex[cInd] = self.i
		    self.i+=1
		itSV.next()
	    
	    itSF = OpenMaya.MItSubdFace(self.dagPath.node())
	    itSF.setLevel( THELEVEL )
	    
	    while not itSF.isDone():
		fInd = itSF.index()
		if self.fShape.polygonHasVertexUVs(fInd):
			vUList = OpenMaya.MDoubleArray()
			vVList = OpenMaya.MDoubleArray()
			self.fShape.polygonGetVertexUVs(fInd, vUList, vVList)
			#OpenMaya.MGlobal.displayWarning( "###########################polygonHasVertexUVs length : %i, %i" % (len(vUList), len(vVList)))
		#else:
		#	OpenMaya.MGlobal.displayWarning( "###########################polygonHasVertexUVs none")
		#if not self.fShape.polygonHasChildren(fInd):
		if not fInd == -1 :
		    pVerts = OpenMaya.MUint64Array()
		    self.fShape.polygonVertices(fInd, pVerts)
		    indCol = []
		    iLoop = 0
		    for pVert in pVerts:
			#OpenMaya.MGlobal.displayWarning( "###########################self.localIndex[pVert] = %i, %i" % (pVert, self.localIndex[pVert]) )
			indCol.append(self.localIndex[pVert])
			self.uvArray[self.localIndex[pVert]].x = vUList[iLoop]
			self.uvArray[self.localIndex[pVert]].y = vVList[iLoop]
			iLoop += 1
		    self.indArray[self.j] = indCol
		self.j+=1
		itSF.next()

        self.addToOutput( '# Subdiv Shape %s' % self.dagPath.fullPathName() )
        self.addToOutput( 'AttributeBegin' )
        self.addToOutput( self.translationMatrix(self.dagPath) )

	self.shadingGroup = self.findShadingGroup()
	self.addToOutput( self.findSurfaceShader( shadingGroup = self.shadingGroup ) )

        self.addToOutput( '\tShape "loopsubdiv"' )
        
	self.addToOutput( '\t"integer nlevels" [%i]' % nSubDLevels)
        self.addToOutput( '\t"integer indices" [' )
        for k in self.indArray:
            self.addToOutput( '\t\t%i %i %i' % (self.indArray[k][0], self.indArray[k][1], self.indArray[k][2]) )
	    if len(self.indArray[k]) >= 4:
		self.addToOutput( '\t\t%i %i %i ' % (self.indArray[k][0], self.indArray[k][2], self.indArray[k][3]) )
        self.addToOutput( '\t]' )
        
        self.addToOutput( '\t"point P" [' )
        for vP in self.vertArray:
            self.addToOutput( '\t\t%f %f %f ' % (self.vertArray[vP].x, self.vertArray[vP].y, self.vertArray[vP].z) )
        self.addToOutput( '\t]' )

        self.addToOutput( '\t"float uv" [' )
        for vUV in self.uvArray:
            self.addToOutput( '\t\t%f %f' % (self.uvArray[vUV].x, self.uvArray[vUV].y) )
        self.addToOutput( '\t]' )

        self.addToOutput( '\t"normal N" [' )
        for vN in self.normArray:
            self.addToOutput( '\t\t%f %f %f ' % (self.normArray[vN].x, self.normArray[vN].y, self.normArray[vN].z) )
        self.addToOutput( '\t]' )
        
        self.addToOutput( 'AttributeEnd' )
        self.addToOutput( '' )
        
        self.fileHandle.flush()