# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: ShaderNode.py,v 1.3 2008/06/16 15:24:54 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( attributes base class )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from GirlNode import GirlNode
from GirlNode import ShaderColorAttribute
from GirlNode import ShaderFloatAttribute
from GirlNode import ShaderEnumAttribute
from GirlNode import ShaderBoolAttribute
from GirlNode import ShaderStringAttribute
#from Girl.GirlMiscModules.FileCollector import FileCollector

# ShaderModules serve dual purpose: Set up the custom shader in Maya
# AND provide attribute->parameter translation during export
class ShaderNode(GirlNode):
    """
    Girl custom shader base class. Each material type derives off this.
    """

    GirlType = str()
    
    #attributes = {}
    

    # SCHUMANN CHECK
    def getMaterial(self, shaderNode, shaderName, meshName, components, setCount ):
        self.attributes['bumpmap'] = ShaderFloatAttribute('normalCamera')
        
        for attr in self.attributes:
            if (
                isinstance( self.attributes[attr], ShaderColorAttribute )
             or isinstance( self.attributes[attr], ShaderFloatAttribute )
                ):
                #OpenMaya.MGlobal.displayInfo("%s attr = %s\n" %(shaderName,attr))
                self.addToOutput( self.attributes[attr].getOutput( attr, shaderNode, shaderName, meshName, components, setCount ) )
                
                
        self.addToOutput( 'MakeNamedMaterial "%s"' % shaderName )
        self.addToOutput( '\t"string type" ["%s"]' % self.GirlType )
        
        for attr in self.attributes:
            if not (
                    isinstance( self.attributes[attr], ShaderColorAttribute )
                 or isinstance( self.attributes[attr], ShaderFloatAttribute )
                    ):
                self.addToOutput( self.attributes[attr].getOutput( attr, shaderNode, shaderName, meshName, components, setCount ) )
            else:
                if self.attributes[attr].exportName == '':
                    self.addToOutput( '\t"texture %s" ["%s.%s"]' % (attr, shaderName, attr) )
                else:
                    self.addToOutput( '\t"texture %s" ["%s"]' % (attr, self.attributes[attr].exportName) )
            

        self.addToOutput( '' )
        return self.outputString