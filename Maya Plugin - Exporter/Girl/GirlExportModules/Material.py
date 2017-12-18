# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# by Doug Hammond 02/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Material.py,v 1.5 2008/06/17 11:22:24 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# material export class.
#
# ------------------------------------------------------------------------------
from maya import cmds
from maya import OpenMaya

from ExportModule import ExportModule

# Import all native material types...
from Girl.GirlNodes.ShaderNode                          import ShaderNode
from Girl.GirlNodes.ShaderNodes.carpaintShader          import carpaintShader
from Girl.GirlNodes.ShaderNodes.glassShader             import glassShader
from Girl.GirlNodes.ShaderNodes.matteShader             import matteShader
from Girl.GirlNodes.ShaderNodes.mattetranslucentShader  import mattetranslucentShader
from Girl.GirlNodes.ShaderNodes.metalShader             import metalShader
from Girl.GirlNodes.ShaderNodes.mirrorShader            import mirrorShader
from Girl.GirlNodes.ShaderNodes.mixShader               import mixShader
from Girl.GirlNodes.ShaderNodes.nullShader              import nullShader
from Girl.GirlNodes.ShaderNodes.plasticShader           import plasticShader
from Girl.GirlNodes.ShaderNodes.roughglassShader        import roughglassShader
from Girl.GirlNodes.ShaderNodes.shinymetalShader        import shinymetalShader
from Girl.GirlNodes.ShaderNodes.substrateShader         import substrateShader
from Girl.GirlNodes.ShaderNodes.mbrdfShader		import mbrdfShader

# ...and the types that we can translate
from Girl.MayaShaderModules.displacementShader         import displacementShader
from Girl.MayaShaderModules.lambertShader              import lambertShader
from Girl.MayaShaderModules.phongShader                import phongShader
from Girl.MayaShaderModules.blinnShader                import blinnShader

from Girl.MayaShaderModules.GirlLambertShader              import GirlLambertShader
from Girl.MayaShaderModules.GirlPhongShader                import GirlPhongShader
from Girl.MayaShaderModules.GirlBlinnShader                import GirlBlinnShader
from Girl.MayaShaderModules.GirlAnisotropicShader          import GirlAnisotropicShader
from Girl.MayaShaderModules.GirlLayeredShader          import GirlLayeredShader
from Girl.MayaShaderModules.GirlPhongEShader          import GirlPhongEShader
from Girl.MayaShaderModules.GirlSurfaceShader          import GirlSurfaceShader
from Girl.MayaShaderModules.GirlUseBackgroundShader          import GirlUseBackgroundShader

class MaterialBase:
    """
    Material type base class.
    """
    
    def __init__(self, shaderNode):
        """
        Set up the node that we're dealing with.
        """
        
        self.dpNode = shaderNode
        self.shaderName = self.dpNode.name()

# TODO global list of exported materials for duplicate detection doesn't work.
ExportedMaterials = []

class Material(ExportModule):
    """
    Material ExportModule. This acts as a factory to return a derived ExportModule
    for the given shader type.
    """
    
    @staticmethod
    def MaterialFactory( dpNode, meshName, components, setCount ):
        """
        The material factory.
        """
        
        # global ExportedMaterials
        
        nodeType = dpNode.typeName()
        
        nodeName = dpNode.name
        
        # TODO this doesn't work
        materialNotExported = nodeName not in ExportedMaterials
        
	#OpenMaya.MGlobal.displayWarning("Shader type %s start" % nodeType )

        if (   (dpNode.classification( nodeType ) == "shader/surface")
            or (dpNode.classification( nodeType ) == "shader/displacement")
           ) \
        and materialNotExported:
            ExportedMaterials.append( nodeName )
            if cmds.getAttr( 'Girl_settings.shader_auto_translation' ) == 1:
                    if nodeType == "GirlShader":
                        return MaterialGirl( dpNode )
                    elif nodeType == "displacementShader":
                        return MaterialDisplacement( dpNode )
                    elif nodeType == "lambert":
                        return MaterialGirlLambert( dpNode, meshName, components, setCount )
                    elif nodeType == "phong":
                        return MaterialGirlPhong( dpNode, meshName, components, setCount )
                    elif nodeType == "blinn":
                        return MaterialGirlBlinn( dpNode, meshName, components, setCount )
                    elif nodeType == "anisotropic":
                        return MaterialGirlAnisotropic( dpNode )
                    elif nodeType == "layeredShader":
                        return MaterialGirlLayeredShader( dpNode )
                    elif nodeType == "oceanShader":
                        return MaterialGirlOceanShader( dpNode )
                    elif nodeType == "phongE":
                        return MaterialGirlPhongE( dpNode )
                    elif nodeType == "rampShader":
                        return MaterialGirlRampShader( dpNode )
                    elif nodeType == "surfaceShader":
                        return MaterialGirlSurfaceShader( dpNode )
                    elif nodeType == "useBackground":
                        return MaterialGirlUseBackground( dpNode )
                    else:
                        OpenMaya.MGlobal.displayWarning("Shader type %s not supported" % nodeType )
                        return False
            else:
                    if nodeType == "GirlShader":
                        # export Girl material directly
                        return MaterialGirl( dpNode )
                    elif nodeType == "displacementShader":
                        # parse displacement shader
                        return MaterialDisplacement( dpNode )
                    elif nodeType == "lambert":
                        # translate lambert -> matte
                        return MaterialLambert( dpNode )
                    elif nodeType == "phong":
                        # translate phong -> plastic
                        return MaterialPhong( dpNode )
                    else:
                        OpenMaya.MGlobal.displayWarning("Shader type %s not supported" % nodeType )
                        return False
        else:
            return False
    #end def MaterialFactory
    
# Warning: Shader type anisotropic not supported # 
# Warning: Shader type hairTubeShader not supported # 
# Warning: Shader type layeredShader not supported # 
# Warning: Shader type oceanShader not supported # 
# Warning: Shader type phongE not supported # 
# Warning: Shader type rampShader not supported # 
# Warning: Shader type shadingMap not supported # 
# Warning: Shader type useBackground not supported # 

class MaterialGirl(ExportModule, MaterialBase):
    """
    The native Girl material type. Responsible for detecting the Girl material
    type and returning the syntax from that module's object.
    """
    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        
    def getOutput(self):
        """
        Essentially another factory method for all the Girl material types.
        """
        
        # switch by Girl material type
        materialTypePlug = self.dpNode.findPlug( "material" )
        materialType = materialTypePlug.asInt()
        
        validMaterial = True
        
        if materialType == 0:
            self.shaderSyntaxModule = carpaintShader()
        elif materialType == 1:
            self.shaderSyntaxModule = glassShader()
        elif materialType == 2:
            self.shaderSyntaxModule = roughglassShader()
        elif materialType == 3:
            self.shaderSyntaxModule = matteShader()
        elif materialType == 4:
            self.shaderSyntaxModule = mattetranslucentShader()
        elif materialType == 5:
            self.shaderSyntaxModule = metalShader()
        elif materialType == 6:
            self.shaderSyntaxModule = shinymetalShader()
        elif materialType == 7:
            self.shaderSyntaxModule = mirrorShader()
        elif materialType == 8:
            self.shaderSyntaxModule = plasticShader()
        elif materialType == 9:
            self.shaderSyntaxModule = substrateShader()
        # 10 is AreaLight, not a material
        elif materialType == 11:
            self.shaderSyntaxModule = mixShaderHandler( self.dpNode )
        elif materialType == 12:
            self.shaderSyntaxModule = nullShader()
        elif materialType == 13:
            self.shaderSyntaxModule = mbrdfShader()
        else:
            validMaterial = False
        
        if validMaterial:
            self.addToOutput( '# Girl Shader Material ' + self.dpNode.name() )
            self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )



# TODO put these routine in *Shader.py as overrides

# Lets do this one manually, because it's an exception
class mixShaderHandler(ExportModule, MaterialBase):
    
    #def __init___(self):
        
        
    def getMaterial(self, dpNode, shaderName):
        self.shaderSyntaxModule = mixShader()
        
        # OK, lets find the input connexions
        nm1Plug = dpNode.findPlug( 'mixNamed1' )
        inputPlugs1 = OpenMaya.MPlugArray()
        nm1Plug.connectedTo(inputPlugs1, True, True)
        
        nm2Plug = dpNode.findPlug( 'mixNamed2' )
        inputPlugs2 = OpenMaya.MPlugArray()
        nm2Plug.connectedTo(inputPlugs2, True, True)
        
        # if both are connected
        if inputPlugs1.length() > 0 and inputPlugs2.length() > 0:
            nmnode1 = inputPlugs1[0].node()
            nmnode2 = inputPlugs2[0].node()
            nm1 = OpenMaya.MFnDependencyNode( nmnode1 ).name()
            nm2 = OpenMaya.MFnDependencyNode( nmnode2 ).name()
            
            # This sets the material name, type and amount value/texture
            #self.addToOutput( self.shaderSyntaxModule.getMaterial( dpNode, shaderName ) )
	    preOutString = self.shaderSyntaxModule.getMaterial( dpNode, shaderName )
            
            # now just grab the names of the connected shaders
            self.addToOutput( '%s\t"string namedmaterial1" ["%s"]' % (preOutString, nm1) )
            self.addToOutput( '\t"string namedmaterial2" ["%s"]' % nm2 )
            
            # and output the dependent nodes before
            #em1 = Material.MaterialFactory( OpenMaya.MFnDependencyNode( nmnode1 ) )
            #self.prependToOutput( em1.loadModule() )
            
            #em2 = Material.MaterialFactory( OpenMaya.MFnDependencyNode( nmnode2 ) )
            #self.prependToOutput( em2.loadModule() )
            
        return self.outputString


class MaterialDisplacement(ExportModule, MaterialBase):
    """
    A translatable Maya shader: displacement
    """
    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = displacementShader()
        
    def getOutput(self):
        """
        Get the syntax from the displacementShader module
        """
        
        self.addToOutput( '# Translated Displacement Shader ' + self.shaderName )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )

class MaterialLambert(ExportModule, MaterialBase):
    """
    A translatable Maya shader: Lambert
    """
    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = lambertShader()
        
    def getOutput(self):
        """
        Get the syntax from the lambertShader module
        """
        
        self.addToOutput( '# Translated Lambert Material ' + self.shaderName )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )

class MaterialPhong(ExportModule, MaterialBase):
    """
    A translatable Maya shader: Phong
    """
    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = phongShader()
        
    def getOutput(self):
        """
        Get the syntax from the phongShader module.
        """
        
        self.addToOutput( '# Translated Phong Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )

class MaterialBlinn(ExportModule, MaterialBase):
    """
    A translatable Maya shader: Blinn
    """
    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = blinnShader()
        
    def getOutput(self):
        """
        Get the syntax from the phongShader module.
        """
        
        self.addToOutput( '# Translated Blinn Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )




class MaterialGirlLambert(ExportModule, MaterialBase):
    """
    A translatable Maya shader: Lambert
    """
    
    def __init__(self, shaderNode, meshName, components, setCount):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlLambertShader()
        self.meshName = meshName
        self.components = components
        self.setCount = setCount
    def getOutput(self):
        """
        Get the syntax from the lambertShader module
        """
        
        self.addToOutput( '# Translated Lambert Material ' + self.shaderName )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName, self.meshName, self.components, self.setCount ) )

class MaterialGirlBlinn(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode, meshName, components, setCount):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlBlinnShader()
        self.meshName = meshName
        self.components = components
        self.setCount = setCount

    def getOutput(self):
        
        self.addToOutput( '# Translated Blinn Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName, self.meshName, self.components, self.setCount ) )


class MaterialGirlPhong(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode, meshName, components, setCount):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlPhongShader()
        self.meshName = meshName
        self.components = components
        self.setCount = setCount
        
    def getOutput(self):
        
        self.addToOutput( '# Translated Phong Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName, self.meshName, self.components, self.setCount ) )


class MaterialGirlAnisotropic(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlAnisotropicShader()
        
    def getOutput(self):
        
        self.addToOutput( '# Translated Anisotropic Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )


class MaterialGirlLayeredShader(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlLayeredShader()
        
    def getOutput(self):
        
        self.addToOutput( '# Translated LayeredShader Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )


class MaterialGirlPhongE(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlPhongEShader()
        
    def getOutput(self):
        
        self.addToOutput( '# Translated PhongE Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )


class MaterialGirlSurfaceShader(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlSurfaceShader()
        
    def getOutput(self):
        
        self.addToOutput( '# Translated SurfaceShader Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )


class MaterialGirlUseBackground(ExportModule, MaterialBase):

    
    def __init__(self, shaderNode):
        MaterialBase.__init__(self, shaderNode)
        self.shaderSyntaxModule = GirlUseBackgroundShader()
        
    def getOutput(self):
        
        self.addToOutput( '# Translated UseBackground Material ' + self.dpNode.name() )
        self.addToOutput( self.shaderSyntaxModule.getMaterial( self.dpNode, self.shaderName ) )
