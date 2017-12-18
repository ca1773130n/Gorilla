# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: lambertShader.py,v 1.1 2008/06/08 12:57:47 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Lambert shader node attributes translator
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute
from Girl.GirlNodes.GirlNode import ShaderEnumAttribute
from Girl.GirlNodes.GirlNode import ShaderBoolAttribute

class GirlLayeredShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    This class provides an interface to translate Maya lambert shaders
    into Girl matte materials.
    """
    strrMatteOpacityMode = {
			0: "Black Hole",
			1: "Solid Matte",
			2: "Opacity Gain"
			}

    strrCompositingFlag = {
			0: "Black Hole",
			1: "Solid Matte",
			2: "Opacity Gain"
			}

    def __init__(self):
        """
        lambertShader constructor. Calls the parent OpenMayaMPx.MPxNode constructor
        and defines the Maya attribute -> Girl material parameter mapping.
        """
        
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for shader
        self.attributes = {}
        self.GirlType = "GirlLayeredShader"
        self.attributes['matteOpacityMode'] = ShaderEnumAttribute('matteOpacityMode', self.strrMatteOpacityMode)
        self.attributes['matteOpacity'] = ShaderFloatAttribute('matteOpacity')

        self.attributes['hardwareColor'] = ShaderColorAttribute('hardwareColor')
        self.attributes['compositingFlag'] = ShaderEnumAttribute('compositingFlag', self.strrCompositingFlag)

        #Hardware Shader
         
    def getMaterial(self, shaderNode, shaderName ):
        
        for attr in self.attributes:
            if (
                isinstance( self.attributes[attr], ShaderColorAttribute )
             or isinstance( self.attributes[attr], ShaderFloatAttribute )
                ):
                #OpenMaya.MGlobal.displayInfo("%s attr = %s\n" %(shaderName,attr))
                self.addToOutput( self.attributes[attr].getOutput( attr, shaderNode, shaderName ) )
                
                
        self.addToOutput( 'MakeNamedMaterial "%s"' % shaderName )
        self.addToOutput( '\t"string type" ["%s"]' % self.GirlType )
        
        for attr in self.attributes:
            if not (
                    isinstance( self.attributes[attr], ShaderColorAttribute )
                 or isinstance( self.attributes[attr], ShaderFloatAttribute )
                    ):
                self.addToOutput( self.attributes[attr].getOutput( attr, shaderNode, shaderName ) )
            else:
                if self.attributes[attr].exportName == '':
                    self.addToOutput( '\t"texture %s" ["%s.%s"]' % (attr, shaderName, attr) )
                else:
                    self.addToOutput( '\t"texture %s" ["%s"]' % (attr, self.attributes[attr].exportName) )
            

        self.addToOutput( '' )
        return self.outputString