# ------------------------------------------------------------------------------
# Girl texture nodes for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: TextureNode.py,v 1.2 2008/06/09 14:45:28 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl Texture node base class
#
# ------------------------------------------------------------------------------

import math

from maya import OpenMaya

from GirlNode import GirlNode

class TextureNode(GirlNode):
    """
    Girl custom texture node base class
    """
    
    def getTexture(self, plugName, textureNode, textureName, attrType ):
        nTextName = '%s.%s' % (textureName, plugName)
        self.addToOutput( 'Texture "%s"' % nTextName )
        self.addToOutput( '\t"%s" "%s"' % ( attrType, self.GirlName() ) )
        
        for attr in self.attributes:
            self.addToOutput( self.attributes[attr].getOutput(attr, textureNode, textureName, self.GirlName() ) )

        return self.outputString
    