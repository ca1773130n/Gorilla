# ------------------------------------------------------------------------------
# Girl texture nodes for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: bilerpTexture.py,v 1.2 2008/06/19 14:43:18 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl BiLerp Texture node for Maya
#
# ------------------------------------------------------------------------------

import math
from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.TextureNode import TextureNode
from Girl.GirlNodes.GirlNode import TextureIntegerAttribute
from Girl.GirlNodes.GirlNode import TextureFloatAttribute
from Girl.GirlNodes.GirlNode import TextureColorAttribute
from Girl.GirlNodes.GirlNode import TextureEnumAttribute
from Girl.GirlNodes.GirlNode import TextureBoolAttribute

# 3D Texture
class EnvBallTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    @staticmethod
    def nodeName():
        return "envBall"
    
    def GirlName(self):
        return "GirlEnvBall"

    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for texture
        self.attributes = {}
        
        self.attributes['eyeSpace']  = TextureBoolAttribute('eyeSpace', self.addToOutput, self.prependToOutput)
        self.attributes['reflect']  = TextureBoolAttribute('reflect', self.addToOutput, self.prependToOutput)

        self.attributes['inclination']  = TextureFloatAttribute('inclination', self.addToOutput, self.prependToOutput)
        self.attributes['elevation']  = TextureFloatAttribute('elevation', self.addToOutput, self.prependToOutput)

        self.attributes['skyRadius']  = TextureFloatAttribute('skyRadius', self.addToOutput, self.prependToOutput)
        self.attributes['bottom']  = TextureFloatAttribute('bottom', self.addToOutput, self.prependToOutput)
        self.attributes['top']  = TextureFloatAttribute('top', self.addToOutput, self.prependToOutput)
        self.attributes['left']  = TextureFloatAttribute('left', self.addToOutput, self.prependToOutput)
        self.attributes['right']  = TextureFloatAttribute('right', self.addToOutput, self.prependToOutput)
        self.attributes['front']  = TextureFloatAttribute('front', self.addToOutput, self.prependToOutput)
        self.attributes['back']  = TextureFloatAttribute('back', self.addToOutput, self.prependToOutput)






