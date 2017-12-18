# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: arealightShader.py,v 1.2 2008/06/13 13:09:51 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( arealight attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode

class arealightShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    AreaLight fragment of GirlShader
    """

    # arealight
    L            =    OpenMaya.MObject()    # color
    gain         =    OpenMaya.MObject()
    numsamples   =    OpenMaya.MObject()

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

    @staticmethod
    def shaderInitializer():
        try:
            # color
            arealightShader.L = arealightShader.makeColor("arealightL", "all")
            arealightShader.gain = arealightShader.makeFloat("arealightGain","aga", 1.0)
            arealightShader.numsamples = arealightShader.makeInteger("arealightNumsamples", "ans", 1)

        except:
            OpenMaya.MGlobal.displayError("Failed to create arealight attributes\n")
            raise
