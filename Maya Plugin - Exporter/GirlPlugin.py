#Maya
from maya import OpenMaya
from maya import OpenMayaMPx
from maya import cmds
from maya import mel

#Commands
#from Girl.GirlMiscModules.geoSunData import updateSunNode

#Nodes
from Girl.GirlNodes.GirlShader import GirlShader
from Girl.GirlNodes.GirlObjectLocator import GirlObjectLocator
from Girl.GirlNodes.GirlEnvironmentLight import GirlEnvironmentLight
from Girl.GirlNodes.GirlSunsky import GirlSunsky

#Textures
from Girl.GirlNodes.TextureNodes.bilerpTexture import bilerpTexture
from Girl.GirlNodes.TextureNodes.blenderCloudsTexture import blenderCloudsTexture
from Girl.GirlNodes.TextureNodes.blenderMarbleTexture import blenderMarbleTexture
from Girl.GirlNodes.TextureNodes.blenderMusgraveTexture import blenderMusgraveTexture
from Girl.GirlNodes.TextureNodes.blenderWoodTexture import blenderWoodTexture
from Girl.GirlNodes.TextureNodes.checkerboard2dTexture import checkerboard2dTexture
from Girl.GirlNodes.TextureNodes.checkerboard3dTexture import checkerboard3dTexture
from Girl.GirlNodes.TextureNodes.dotsTexture import dotsTexture
from Girl.GirlNodes.TextureNodes.fbmTexture import fbmTexture
from Girl.GirlNodes.TextureNodes.marbleTexture import marbleTexture
from Girl.GirlNodes.TextureNodes.mixTexture import mixTexture
from Girl.GirlNodes.TextureNodes.scaleTexture import scaleTexture
from Girl.GirlNodes.TextureNodes.windyTexture import windyTexture
from Girl.GirlNodes.TextureNodes.wrinkledTexture import wrinkledTexture

#GUI/Commands
from Girl.GirlCommands.Girl_gui    import Girl_gui
from Girl.GirlCommands.Girl_batch   import Girl_batch
from Girl.GirlCommands.Girl_render   import Girl_render

# ------------------------------------------------------------------------------

# Global list of commands that we want to register with Maya.
GirlCommands= [
			  Girl_gui,
			  Girl_batch,
			  Girl_render
			 ]

# We need to know the node types when registering, so provide a dict
# of the nodes and their types.
GirlNodes = {
		    # shaders
			GirlShader: OpenMayaMPx.MPxNode.kDependNode,
			
			# locators / lights
			GirlObjectLocator: OpenMayaMPx.MPxNode.kLocatorNode,
			GirlEnvironmentLight: OpenMayaMPx.MPxNode.kLocatorNode,
			GirlSunsky: OpenMayaMPx.MPxNode.kLocatorNode,
			
			# textures
			bilerpTexture: OpenMayaMPx.MPxNode.kDependNode,
			blenderCloudsTexture: OpenMayaMPx.MPxNode.kDependNode,
			blenderMarbleTexture: OpenMayaMPx.MPxNode.kDependNode,
			blenderMusgraveTexture: OpenMayaMPx.MPxNode.kDependNode,
			blenderWoodTexture: OpenMayaMPx.MPxNode.kDependNode,
			checkerboard2dTexture: OpenMayaMPx.MPxNode.kDependNode,
			checkerboard3dTexture: OpenMayaMPx.MPxNode.kDependNode,
			dotsTexture: OpenMayaMPx.MPxNode.kDependNode,
			fbmTexture: OpenMayaMPx.MPxNode.kDependNode,
			marbleTexture: OpenMayaMPx.MPxNode.kDependNode,
			mixTexture: OpenMayaMPx.MPxNode.kDependNode,
			scaleTexture: OpenMayaMPx.MPxNode.kDependNode,
			windyTexture: OpenMayaMPx.MPxNode.kDependNode,
			wrinkledTexture: OpenMayaMPx.MPxNode.kDependNode
		   }

# initialize the script plug-ins
def initializePlugin(mobject):
	"""
	Start the plugin by registering all commands and nodes etc that this plugin provides
	"""
	
	mplugin = OpenMayaMPx.MFnPlugin(mobject, "Girl [ETRI]", "1.0", "Any")
	OpenMaya.MGlobal.displayInfo('start seting')

	try:

		# Register commands
		for command in GirlCommands:
			mplugin.registerCommand( command.commandName(), command.commandCreator )

		#Register nodes
		for node in GirlNodes:
			mplugin.registerNode( node.nodeName(),
								  node.nodeId(),
								  node.nodeCreator,
								  node.nodeInitializer,
								  GirlNodes[node],
								  node.nodeClassify() )
		
		if OpenMaya.MGlobal.mayaState() == OpenMaya.MGlobal.kInteractive:
			# Create Girl menu
			lg = Girl_gui()
			lg.makeMainMenu()
			#OpenMaya.MGlobal.executeCommand("renderer -rendererUIName \"GIRL Renderer\" -renderProcedure \"Girl_render\" Girl")
		else:
			OpenMaya.MGlobal.displayInfo('GirlMaya: Plugin loaded in console mode')

		# OpenMaya.MGlobal.displayInfo("Plugin Loaded OK.")
	except:
		OpenMaya.MGlobal.displayError( "Failed to register Girl Plugin" )
		raise

# uninitialize the script plug-in
def uninitializePlugin(mobject):
	"""
	Unregister all commands and nodes etc that the initializePlugin() method registered with Maya
	"""
	 
	mplugin = OpenMayaMPx.MFnPlugin(mobject)
	try:
		# deregister commands
		for command in GirlCommands:
			mplugin.deregisterCommand ( command.commandName() )
	
		# deregister nodes
		for node in GirlNodes:
			mplugin.deregisterNode( node.nodeId() )

		# OpenMaya.MGlobal.displayInfo("Plugin Removed OK.")
	except:
		OpenMaya.MGlobal.displayError( "Failed to deregister plugin" )
		raise
		
		