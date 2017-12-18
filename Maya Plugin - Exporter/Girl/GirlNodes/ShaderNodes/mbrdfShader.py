# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: carpaintShader.py,v 1.1 2008/06/08 12:57:42 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( mbrdfShader attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderEnumAttribute
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute
from Girl.GirlNodes.GirlNode import ShaderStringHideAttribute


class mbrdfShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    mbrdfShader fragment of GirlShader
    """
    
    # mbrdf
    Name			=    OpenMaya.MObject()    # name
    MaterialName		=    OpenMaya.MObject()    # 
    MaterialSubName		=    OpenMaya.MObject()    # enum
    cr		=    OpenMaya.MObject()    # r
    cg		=    OpenMaya.MObject()    # g
    cb		=    OpenMaya.MObject()    # b
    
    mBrdfNameValues = {
			0: "alum-bronze",
			1: "alumina-oxide",
			2: "aluminium",
			3: "aventurnine",
			4: "beige-fabric",
			5: "black-fabric",
			6: "black-obsidian",
			7: "black-oxidized-steel",
			8: "black-phenolic",
			9: "black-soft-plastic",
			10: "blue-acrylic",
			11: "blue-fabric",
			12: "blue-metallic-paint",
			13: "blue-metallic-paint2",
			14: "blue-rubber",
			15: "brass",
			16: "cherry-235",
			17: "chrome-steel",
			18: "chrome",
			19: "colonial-maple-223",
			20: "color-changing-paint1",
			21: "color-changing-paint2",
			22: "color-changing-paint3",
			23: "dark-blue-paint",
			24: "dark-red-paint",
			25: "dark-specular-fabric",
			26: "delrin",
			27: "fruitwood-241",
			28: "gold-metallic-paint",
			29: "gold-metallic-paint2",
			30: "gold-metallic-paint3",
			31: "gold-paint",
			32: "gray-plastic",
			33: "grease-covered-steel",
			34: "green-acrylic",
			35: "green-fabric",
			36: "green-latex",
			37: "green-metallic-paint",
			38: "green-metallic-paint2",
			39: "green-plastic",
			40: "hematite",
			41: "ipswich-pine-221",
			42: "light-brown-fabric",
			43: "light-red-paint",
			44: "maroon-plastic",
			45: "natural-209",
			46: "neoprene-rubber",
			47: "nickel",
			48: "nylon",
			49: "orange-paint",
			50: "pearl-paint",
			51: "pickled-oak-260",
			52: "pink-fabric",
			53: "pink-fabric2",
			54: "pink-felt",
			55: "pink-jasper",
			56: "pink-plastic",
			57: "polyethylene",
			58: "polyurethane-foam",
			59: "pure-rubber",
			60: "purple-paint",
			61: "pvc",
			62: "red-fabric",
			63: "red-fabric2",
			64: "red-metallic-paint",
			65: "red-phenolic",
			66: "red-plastic",
			67: "red-specular-plastic",
			68: "silicon-nitrade",
			69: "silver-metallic-paint",
			70: "silver-metallic-paint2",
			71: "silver-paint",
			72: "special-walnut-224",
			73: "specular-black-phenolic",
			74: "specular-blue-phenolic",
			75: "specular-green-phenolic",
			76: "specular-maroon-phenolic",
			77: "specular-orange-phenolic",
			78: "specular-red-phenolic",
			79: "specular-violet-phenolic",
			80: "specular-white-phenolic",
			81: "specular-yellow-phenolic",
			82: "ss440",
			83: "steel",
			84: "teflon",
			85: "tungsten-carbide",
			86: "two-layer-gold",
			87: "two-layer-silver",
			88: "violet-acrylic",
			89: "violet-rubber",
			90: "white-acrylic",
			91: "white-diffuse-bball",
			92: "white-fabric",
			93: "white-fabric2",
			94: "white-marble",
			95: "white-paint",
			96: "yellow-matte-plastic",
			97: "yellow-paint",
			98: "yellow-phenolic",
			99: "yellow-plastic"
		}

    mBrdfMaterialNameValues = {
			0: "MerlBRDF",
			1: "MerlBRDFFit",
			2: "Analytic-Models"
			}

    mBrdfMaterialSubNameValues = {
			0: "MerlLafortune",
			1: "MerlCookTorrance",
			2: "Ward",
			3: "Ward-Duer",
			4: "Blinn-Phong",
			5: "Ashikmin-Shirely",
			6: "He"
			}

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # Export attributes
        self.attributes = {}
        self.GirlType = "MerlBRDF"
        self.attributes['MerlBRDFName'] = ShaderEnumAttribute('mBrdfNames', self.mBrdfNameValues)
        self.attributes['Material'] = ShaderEnumAttribute('mBrdfMaterialNames', self.mBrdfMaterialNameValues)
        self.attributes['MaterialSub'] = ShaderEnumAttribute('mBrdfMaterialSubNames', self.mBrdfMaterialSubNameValues)
        self.attributes['reflection'] = ShaderStringHideAttribute('reflection', 'all')
        self.attributes['cr'] = ShaderFloatAttribute('cr')
        self.attributes['cg'] = ShaderFloatAttribute('cg')
        self.attributes['cb'] = ShaderFloatAttribute('cb')

    @staticmethod
    def shaderInitializer():
        enumAttr    = OpenMaya.MFnEnumAttribute()

        try:
            # preset name (or manual)
            mbrdfShader.Name = enumAttr.create("mBrdfNames", "mBrdfN", 0)
            for ind in mbrdfShader.mBrdfNameValues:
                enumAttr.addField( mbrdfShader.mBrdfNameValues[ind], ind )

            mbrdfShader.MaterialName = enumAttr.create("mBrdfMaterialNames", "mBrdfMN", 0)
            for ind in mbrdfShader.mBrdfMaterialNameValues:
                enumAttr.addField( mbrdfShader.mBrdfMaterialNameValues[ind], ind )

            mbrdfShader.MaterialSubName = enumAttr.create("mBrdfMaterialSubNames", "mBrdfMSN", 0)
            for ind in mbrdfShader.mBrdfMaterialSubNameValues:
                enumAttr.addField( mbrdfShader.mBrdfMaterialSubNameValues[ind], ind )

            mbrdfShader.cr = mbrdfShader.makeFloat("cr", "colorR", 1.0)
            mbrdfShader.cg = mbrdfShader.makeFloat("cg", "colorG", 1.0)
	    mbrdfShader.cb = mbrdfShader.makeFloat("cb", "colorB", 1.0)

        except:
            OpenMaya.MGlobal.displayError("Failed to create mbrdf attributes\n")
            raise

    # SCHUMANN CHECK
    def getMaterial(self, shaderNode, shaderName ):
        self.attributes['bumpmap'] = ShaderFloatAttribute('normalCamera')
        
               
        for attr in self.attributes:
            if (
                isinstance( self.attributes[attr], ShaderColorAttribute )
             or isinstance( self.attributes[attr], ShaderFloatAttribute )
                ):

                if (attr != 'cr' and attr != 'cg' and attr != 'cb') :
			self.addToOutput( self.attributes[attr].getOutput( attr, shaderNode, shaderName ) )

        self.addToOutput( 'MakeNamedMaterial "%s"' % shaderName )
        #self.addToOutput( '\t"string type" ["%s"]' % self.GirlType )
	strMetName = self.attributes['Material'].getStringValue( 'Material', shaderNode, shaderName )
	
	if ( strMetName == 'MerlBRDF' or strMetName == 'MerlBRDFFit' ):
		self.addToOutput( '\t"string type" ["%s"]' % strMetName )
	else:
		self.addToOutput( '\t"string type" ["%s"]' % self.attributes['MaterialSub'].getStringValue( 'MerlBRDFName', shaderNode, shaderName ) )

        self.addToOutput( '\t"string merltype" ["%s"]' % self.attributes['MerlBRDFName'].getStringValue( 'MerlBRDFName', shaderNode, shaderName ) )
	self.addToOutput( '\t"string reflection" ["all"]')
        
        fcr = self.attributes['cr'].getFloatValue( shaderNode, shaderName )
        fcg = self.attributes['cg'].getFloatValue( shaderNode, shaderName )
        fcb = self.attributes['cb'].getFloatValue( shaderNode, shaderName )

	self.addToOutput( '\t"float r" [%f]' % fcr )
	self.addToOutput( '\t"float g" [%f]' % fcg )
	self.addToOutput( '\t"float b" [%f]' % fcb )

        for attr in self.attributes:
            if  (
                    isinstance( self.attributes[attr], ShaderColorAttribute )
                 or isinstance( self.attributes[attr], ShaderFloatAttribute )
                    ):
                if (attr != 'cr' and attr != 'cg' and attr != 'cb') :
			if self.attributes[attr].exportName == '':
			    self.addToOutput( '\t"texture %s" ["%s.%s"]' % (attr, shaderName, attr) )
			else:
			    self.addToOutput( '\t"texture %s" ["%s"]' % (attr, self.attributes[attr].exportName) )

        self.addToOutput( '' )
        return self.outputString