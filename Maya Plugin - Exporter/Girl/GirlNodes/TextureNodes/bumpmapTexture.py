# ------------------------------------------------------------------------------
# Girl texture nodes for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: bumpmapTexture.py,v 1.3 2008/06/09 17:31:35 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl Bump Map expoter
#
# ------------------------------------------------------------------------------

import math, os
from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.TextureNode import TextureNode
from Girl.GirlNodes.GirlNode import TextureStringAttribute
from Girl.GirlNodes.GirlNode import TextureFloatAttribute

# from Girl.GirlMiscModules.FileCollector import FileCollector

# dummy Texture
class bumpmapTexture(TextureNode):
    """
    Bump map Texture node handler
    """
    
#    @staticmethod
#    def nodeName():
#        return "fileTexture"
    
    def GirlName(self):
        return 'scale'
    
    def __init__(self):
        
        self.attributes = {}
        self.attributes['tex1'] = TextureFloatAttribute('bumpValue', self.addToOutput, self.prependToOutput)
        self.attributes['tex2'] = TextureFloatAttribute('bumpDepth', self.addToOutput, self.prependToOutput) 


#    def getTexture(self, plugName, shaderNode, shaderName, attrType ):
#        bumpPlug = shaderNode.findPlug('bumpValue')
#        dgIt = OpenMaya.MItDependencyGraph( bumpPlug,
#                                            OpenMaya.MFn.kFileTexture,
#                                            OpenMaya.MItDependencyGraph.kUpstream,
#                                            OpenMaya.MItDependencyGraph.kBreadthFirst,
#                                            OpenMaya.MItDependencyGraph.kNodeLevel
#                                            )
#        
#        dgIt.disablePruningOnFilter()
#        
#        if not dgIt.isDone():
#            hasBumpMap = True
#            
#            textureNode = dgIt.thisNode()
#            fnTextureNode = OpenMaya.MFnDependencyNode(textureNode)
#            filenamePlug = fnTextureNode.findPlug('fileTextureName')
#            bumpImage = filenamePlug.asString() 
#        
#            colorgainPlug = shaderNode.findPlug( 'bumpDepth' )
#            #colorgainPlug = fnTextureNode.findPlug('colorGainR')
#            bumpScale = colorgainPlug.asFloat() 
#            
#            self.addToOutput( 'Texture "' + shaderName + '.normalCamera.scale"' )
#            self.addToOutput( '\t"float" "constant"' )
#            self.addToOutput( ( '\t\t"float value" [%f]' % bumpScale ) )
#                
#            self.addToOutput( 'Texture "' + shaderName + '.normalCamera.image"' )
#            self.addToOutput( '\t"float" "imagemap"' )
#            self.addToOutput( '\t\t"float vscale" [-1.0]' )
#            self.addToOutput( ( '\t\t"string filename" ["%s"]' % FileCollector.collectBumpmap( bumpImage ) ) )
#            
#            self.addToOutput( 'Texture "' + shaderName + '.normalCamera"' )
#            self.addToOutput( '\t"float" "scale"' )
#            self.addToOutput( ( '\t\t"texture tex1" ["%s.normalCamera.scale"]' % shaderName ) )
#            self.addToOutput( ( '\t\t"texture tex2" ["%s.normalCamera.image"]' % shaderName ) )
#            
#        return self.outputString