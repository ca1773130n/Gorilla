# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: MeshOpt.py,v 1.29 2008/06/27 12:30:13 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# trianglemesh/loopsubdiv geometry export module (optimised version)
#
# ------------------------------------------------------------------------------

import time, os
from maya import OpenMaya
from maya import cmds

from ExportModule import ExportModule
import Girl.GirlExportModules.Material        as GirlModuleMaterial

g_MaterialNames = []
g_AreaSizes = []
g_Sets = []

class MeshOpt(ExportModule):
    """
    Polygon mesh ExportModule (Optimised)
    """

    doBenchmark = False
    doProfiling = False

    fShape = OpenMaya.MFnMesh()
    isInstanced = False
    instanceNum = 0
    
    hasUVs = False
    UVSets = []
    currentUVSet = 0
    
    vertNormUVList = {}
    vertIndexList = []
    vertPointList = []
    vertNormList = []
    vertUVList = []
    
    fileHandle = int()
    
    # used to determine appropriate UV and Normals output
    mode = 'trianglemesh' # or loopsubdiv
    type = 'geom' # or portal

    def __init__(self, fileHandles, dagPath, dagFullPath):
        
        self.dagPath = dagPath
        self.fShape = OpenMaya.MFnMesh( dagPath )
        self.dagFullPath = dagFullPath
        meshHandle, portalsHandle = fileHandles
        self.portalsHandle = portalsHandle
        
        if dagFullPath.lower().find('portal') != -1:
            self.fileHandle = portalsHandle
            self.type = 'portal'
        else:
            self.fileHandle = meshHandle
            self.type = 'geom'
            
        dagPath.extendToShape()

        if dagPath.isInstanced():
            self.isInstanced = True
            self.instanceNum = dagPath.instanceNumber()
            
        self.fPolygonSets = OpenMaya.MObjectArray()
        self.fPolygonComponents = OpenMaya.MObjectArray()
        self.fShape.getConnectedSetsAndMembers(self.instanceNum, self.fPolygonSets, self.fPolygonComponents, True)
        
        shaderArray = OpenMaya.MObjectArray()
        polyShaderIdx = OpenMaya.MIntArray()
        
        self.setCount = 0
        self.setCount = self.fPolygonSets.length()
        
        #if self.setCount > 1:
         #   self.setCount -= 1
            
        if self.fShape.numUVSets() > 0:
            # Get UV sets for this mesh
            self.UVSets = []
            self.fShape.getUVSetNames( self.UVSets )
            if len(self.UVSets) > 0:
                self.hasUVs = True
        
           
    def resetLists(self):
        # reset lists
        self.vertNormUVList = {}
        self.vertIndexList = []
        self.vertPointList = []
        self.vertNormList = []
        self.vertUVList = []
        
    def deleteLists(self):
        del self.vertNormUVList
        del self.vertIndexList
        del self.vertPointList
        del self.vertNormList
        del self.vertUVList

    def getOutput(self):
        
        if self.doProfiling:
            import hotshot, hotshot.stats
            prof = hotshot.Profile("meshopt.prof")
            benchtime = prof.runcall(self.getOutput_real)
            prof.close()
            stats = hotshot.stats.load("meshopt.prof")
            stats.strip_dirs()
            stats.sort_stats('time','calls')
            stats.print_stats(20)
        else:
            self.getOutput_real()
        

    def getObjectOrInstance(self, iSet, components):
        
        if self.isInstanced:
            if self.instanceNum == 0:
                self.addToOutput( '# Polygon Shape %s (set %i, instanced)' % (self.dagPath.fullPathName(), iSet ) )
                self.addToOutput( 'ObjectBegin "%s"' % (self.fShape.name()) )
                self.getGeometry(iSet)
                self.addToOutput( 'ObjectEnd' )
                self.addToOutput( '' )
                self.fileHandle.flush()
                
            self.addToOutput( '# Polygon Shape %s (set %i, instance %i)' % (self.dagPath.fullPathName(), iSet, self.instanceNum ) )
            self.addToOutput( 'AttributeBegin' )
            self.addToOutput( self.translationMatrix(self.dagPath) )
            self.addToOutput( '\tObjectInstance "%s"' % (self.fShape.name()) )            
            self.addToOutput( 'AttributeEnd' )
            self.addToOutput( '' )
            self.fileHandle.flush()
                
        else:
            self.addToOutput( '# Polygon Shape %s (set %i)' % (self.dagPath.fullPathName(), iSet ) )
            self.addToOutput( 'AttributeBegin' )
            self.addToOutput( self.translationMatrix(self.dagPath) )
                        
            self.getGeometry(iSet, components)
                
            self.addToOutput( 'AttributeEnd' )
            self.addToOutput( '' )
            self.fileHandle.flush()

    def getOutput_real(self):

        # each set/shader on this object
        for iSet in range(0, self.setCount):
            
            if self.setCount > 1:
                skipThisSet = False
                try:
                    fComponent = OpenMaya.MFnComponent( self.fPolygonComponents[iSet] )
                    skipThisSet = fComponent.isEmpty()
                except:
                    skipThisSet = True
                    
                if skipThisSet:
                    OpenMaya.MGlobal.displayWarning( "Skipping empty set %s : %i" % (self.fShape.name(), iSet) )
                    continue
            
            # start afresh for this set
            self.resetLists()

            self.getObjectOrInstance(iSet, self.fPolygonComponents[iSet]) 
                
            self.deleteLists()
            
    def GetSizeOfTriangle(self, p1, p2, p3):
        vB = OpenMaya.MVector( p1 - p2 )
        vC = OpenMaya.MVector( p3 - p2 )
        vBB = OpenMaya.MVector( p2 )
        theta = math.acos( (vB * vC) / (vB.length() * vC.length()) )
        res = ( 0.5 * vB.length() * vC.length() * math.sin(theta) )
        return res
  
    def getGeometry(self, iSet, components):
        
        # get all object verts
        meshPoints = OpenMaya.MPointArray()
        self.fShape.getPoints(meshPoints)
        
        # get all object normals
        meshNormals = OpenMaya.MFloatVectorArray()
        self.fShape.getNormals(meshNormals)
        
        # get all object UVs, if any
        if self.hasUVs:
            try:
                meshUArray = OpenMaya.MFloatArray()
                meshVArray = OpenMaya.MFloatArray()
                self.fShape.getUVs(meshUArray, meshVArray, self.UVSets[self.currentUVSet])
            except:
                OpenMaya.MGlobal.displayError("Error with UV mapping on object: %s" % self.fShape.name() )
                raise
        
        # set up some scripting junk
        numTrianglesPx = OpenMaya.MScriptUtil()
        numTrianglesPx.createFromInt(0)
        numTrianglesPtr = numTrianglesPx.asIntPtr()
        uvIdxPx = OpenMaya.MScriptUtil()
        uvIdxPx.createFromInt(0)
        uvIdxPtr = uvIdxPx.asIntPtr()
        
        # set up mesh face iterator            
        itMeshPolys = OpenMaya.MItMeshPolygon(self.dagPath, self.fPolygonComponents[iSet])

        if not itMeshPolys.hasValidTriangulation():
            OpenMaya.MGlobal.displayWarning( "Shape %s has invalid triangulation, skipping" % self.fShape.name() )
            return
        
        # storage for obj-relative vert indices in a face
        polygonVertices = OpenMaya.MIntArray()
        
        # storage for the face vert points
        vertPoints = OpenMaya.MPointArray()
                    
        # storage for the face vert indices
        vertIndices = OpenMaya.MIntArray()
        
        """
        calculate area size
        
        fSize = 0
        while not itMeshPolys.isDone():
            indices = OpenMaya.MIntArray()            
            itMeshPolys.getVertices( indices )
            sizeIndices = indices.length()
            
            numTri = OpenMaya.MInt()
            itMeshPolys.numTriangles( numTri )
            
            points = OpenMaya.MPointArray()
            for curTri in range(0, numTri):
                itMeshPolys.getTriangle( curTri, points, indices )
                fSize += GetSizeOfTriangle( meshPoints[ indices[0] ], meshPoints[ indices[1] ], meshPoints[ indices[2] ] )
        """                
        
        # set syntax for trianglemesh/loopsubdiv or portalshape
        if self.type == 'geom':
            # detect Material or AreaLight
            self.shadingGroup = self.findShadingGroup(self.instanceNum, iSet)
            #self.addToOutput( self.findSurfaceShader( shadingGroup = self.shadingGroup ) )
            """
            """
            surfaceShader = self.shadingGroup.findPlug("surfaceShader")
            materials = OpenMaya.MPlugArray()
            surfaceShader.connectedTo(materials, True, True)
            
            numMats = materials.length()
            for curMaterial in range(0, numMats):
                matNode = materials[curMaterial].node()
                theMaterial = OpenMaya.MFnDependencyNode( matNode )                
                #dupMaterial = self.dagFullPath.replace('|', '_')
                #dupMaterial = dupMaterial.replace(':', '_')
                
                # reduce the redundancy
                areaSizeMesh = cmds.getAttr( self.dagFullPath + ".as" )
                numMatData = len(g_AreaSizes)
                
                bFound = False
                for iterAS in range(0, numMatData):
                    if g_AreaSizes[iterAS] == areaSizeMesh and g_Sets[iterAS] == iSet:
                        self.addToOutput( 'NamedMaterial "%s"' % g_MaterialNames[iterAS] )
                        bFound = True
                        break
                    
                if bFound == False:                                                
                    dupMaterial = theMaterial.name() + "_1"
                    dupMaterial = dupMaterial.replace(':', '_')
                    while cmds.objExists(dupMaterial):
                        sss = dupMaterial.split("_")
                        dupMaterial = "%s_%d" % (theMaterial.name(), int(sss[len(sss) - 1]) + 1)
                        dupMaterial = dupMaterial.replace(':', '_')                                        
                    
		    print "theMaterial: " + theMaterial.name()
		    print "dupMaterial: " + dupMaterial
                    if theMaterial.name() == 'lambert1':
                        dupMaterial = cmds.shadingNode('lambert', asShader=True)
                    else:
                        cmds.duplicate( theMaterial.name(), n=dupMaterial, un=True, ic=True, rr=True, ilf=True, po=True )
                    
                    selection = OpenMaya.MSelectionList()
                    selection.add(dupMaterial)
                    dupMatNode = OpenMaya.MObject()
                    selection.getDependNode(0, dupMatNode)
                    dupMatDepNode = OpenMaya.MFnDependencyNode( dupMatNode )    
                    expModule = GirlModuleMaterial.Material.MaterialFactory( dupMatDepNode, self.dagFullPath, itMeshPolys, self.setCount )
                    if expModule != False:
                        expOut = expModule.loadModule()
                        expModule.writeTo( self.portalsHandle )
                    #cmds.delete(dupMaterial)
                    
                    self.addToOutput( 'NamedMaterial "%s"' % dupMaterial )
                    
                    g_AreaSizes.append( areaSizeMesh )
                    g_MaterialNames.append( dupMaterial )
                    g_Sets.append( iSet )               
                    
            
            # detect trianglemesh/loopsubdiv
            subPlug1 = self.fShape.findPlug('useMaxSubdivisions')
            useLoopSubdiv = subPlug1.asBool()
            if useLoopSubdiv:
                self.mode = 'loopsubdiv'
                subPlug2 = self.fShape.findPlug('maxSubd')
                nlevels = subPlug2.asInt()
                self.addToOutput( '\tShape "loopsubdiv"' )
                self.addToOutput( '\t\t"integer nlevels" [%i]' % nlevels )
                # find displacement, if any
                self.addToOutput( self.findDisplacementShader( self.shadingGroup ) )
            else:
                self.mode = 'trianglemesh'                
                self.addToOutput( '\tShape "trianglemesh"' )
        else:
            self.addToOutput( '\tPortalShape "trianglemesh"' )
        
        itMeshPolys.reset()
                    
        def compileWithUVs():
            totalVertIndices = 0
            # each face
            while not itMeshPolys.isDone():
                
                # get number of triangles in face
                itMeshPolys.numTriangles(numTrianglesPtr)
                numTriangles = OpenMaya.MScriptUtil(numTrianglesPtr).asInt()
                
                itMeshPolys.getVertices( polygonVertices )

                # each triangle in each face
                for currentTriangle in range(0, numTriangles):

                    # get the triangle points and indices
                    itMeshPolys.getTriangle( currentTriangle, vertPoints, vertIndices, OpenMaya.MSpace.kObject )
                    
                    # get a list of local indices
                    localIndex = self.GetLocalIndex( polygonVertices, vertIndices )
                    
                    # each vert in this triangle
                    for i, vertIndex in enumerate( vertIndices ):
                        
                        # get indices to points/normals/uvs
                        vertNormalIndex = itMeshPolys.normalIndex( localIndex[i] )
                        
                        try:
                            itMeshPolys.getUVIndex( localIndex[i], uvIdxPtr, self.UVSets[self.currentUVSet] )
                            vertUVIndex = OpenMaya.MScriptUtil( uvIdxPtr ).asInt()
                        except:
                            OpenMaya.MGlobal.displayWarning( 'Invalid UV data on object %s (UV set "%s"), restarting object export without UVs' % (self.dagPath.fullPathName(), self.UVSets[self.currentUVSet]) )
                            self.resetLists()
                            itMeshPolys.reset()
                            compileWithoutUVs()
                            self.hasUVs = False
                            return
                        
                        # if we've seen this combo before,
                        testVal = (vertIndex, vertNormalIndex, vertUVIndex) 
                        #try:
                        if testVal in self.vertNormUVList:
                            self.vertIndexList.append( self.vertNormUVList[testVal] )
                        #except KeyError:
                        else:
                            # add it to the lists
                            self.vertPointList.append( meshPoints[vertIndex] )
                            self.vertNormList.append( meshNormals[vertNormalIndex] )
                            self.vertUVList.append( ( meshUArray[vertUVIndex], meshVArray[vertUVIndex] ) )
                            
                            # and keep track of what we've seen
                            self.vertNormUVList[testVal] = totalVertIndices
                            # and use the most recent idx value
                            self.vertIndexList.append( totalVertIndices )
                            totalVertIndices += 1
                        
                itMeshPolys.next()
                
        def compileWithoutUVs():
            totalVertIndices = 0
            # each face
            while not itMeshPolys.isDone():
                
                # get number of triangles in face
                itMeshPolys.numTriangles(numTrianglesPtr)
                numTriangles = OpenMaya.MScriptUtil(numTrianglesPtr).asInt()
                
                itMeshPolys.getVertices( polygonVertices )

                # each triangle in each face
                for currentTriangle in range(0, numTriangles):

                    # get the triangle points and indices
                    itMeshPolys.getTriangle( currentTriangle, vertPoints, vertIndices, OpenMaya.MSpace.kObject )
                    
                    # get a list of local indices
                    localIndex = self.GetLocalIndex( polygonVertices, vertIndices )
                    
                    # each vert in this triangle
                    for i, vertIndex in enumerate( vertIndices ):
                        
                        # get indices to points/normals/uvs
                        vertNormalIndex = itMeshPolys.normalIndex( localIndex[i] )

                        # if we've seen this combo yet,
                        testVal = (vertIndex, vertNormalIndex)
                        #try:
                        if testVal in self.vertNormUVList:
                            self.vertIndexList.append( self.vertNormUVList[testVal] )
                        #except KeyError:
                        else:
                            # add it to the lists
                            self.vertPointList.append( meshPoints[vertIndex] )
                            self.vertNormList.append( meshNormals[vertNormalIndex] )

                            # and keep track of what we've seen
                            self.vertNormUVList[testVal] = totalVertIndices
                            # and use the most recent idx value
                            self.vertIndexList.append( totalVertIndices )
                            totalVertIndices += 1
                        
                        
                itMeshPolys.next()
                
                
        startTime = time.clock()
        
        if itMeshPolys.hasUVs():
            compileWithUVs()
        else:
            compileWithoutUVs()
            
        procTime = time.clock()
        procDuration = procTime - startTime
        
        
        # mesh iteration done, do output.

        self.addToOutput( '\t"integer indices" [' )
        self.addToOutput( '\t\t' + ' '.join(map(str,self.vertIndexList)) )
        self.addToOutput( '\t]' )
        
        self.fileHandle.flush()
        
        self.addToOutput( '\t"point P" [' )
        for vP in self.vertPointList:
            self.addToOutput( '\t\t%f %f %f' % (vP.x, vP.y, vP.z) )
        self.addToOutput( '\t]' )
        
        self.fileHandle.flush()
        
        # add UVs for trianglemesh and loopsubdiv, but not for portals and only if the shape has uvs
        if self.type == 'geom' and itMeshPolys.hasUVs() and len(self.vertUVList)>0:
            self.addToOutput( '\t"float uv" [' )
            for uv in self.vertUVList:
                self.addToOutput( '\t\t%f %f' % uv )
            self.addToOutput( '\t]' )
        
        self.fileHandle.flush()
        
        # Add normals to trianglemesh
        if self.mode == 'trianglemesh':
            self.addToOutput( '\t"normal N" [' )
            for vN in self.vertNormList:
                self.addToOutput( '\t\t%f %f %f' % (vN.x, vN.y, vN.z) )
            self.addToOutput( '\t]' )
            
            
        outTime = time.clock()
        writeDuration = outTime - procTime
        
        if self.doBenchmark:
            vLen = len(self.vertNormUVList)
            pSpeed = vLen/procDuration
            wSpeed = vLen/writeDuration
            print "%i verts processed in %f seconds: %f verts/sec" % (vLen, procDuration, pSpeed)
            print " -> written in %f seconds: %f verts/sec" % (writeDuration, wSpeed)
            
            sf = open("e:\meshopt_stats.csv", "a")
            sf.write ( ( '%i,%f,%f' % (vLen, pSpeed, wSpeed) ) + os.linesep )
            sf.close()
            
    def GetLocalIndex(self, getVertices, getTriangle):
        """
        To quote the C++ source:
            // MItMeshPolygon::getTriangle() returns object-relative vertex
            // indices; BUT MItMeshPolygon::normalIndex() and ::getNormal() need
            // face-relative vertex indices! This converts vertex indices from
            // object-relative to face-relative.
        """
        
        localIndex = []
        
        for gt in range(0, getTriangle.length()):
            for gv in range(0, getVertices.length()):
                if getTriangle[gt] == getVertices[gv]:
                    localIndex.append( gv )
                    break
                    
            if len(localIndex) == gt:
                localIndex.append( -1 )
                
        return localIndex