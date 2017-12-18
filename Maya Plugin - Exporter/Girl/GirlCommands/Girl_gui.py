# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Girl_gui.py,v 1.14 2008/07/06 14:46:23 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# GUI builder command
#
# ------------------------------------------------------------------------------

import os
os.altsep = '/'
from maya import OpenMaya
from maya import OpenMayaMPx
from maya import cmds
from maya import mel

from Girl.GirlMiscModules.fn_attr import fn_attr

from Girl_settings import Girl_settings
from Girl_batch	 import Girl_batch

class mMenu:
	"""
	GUI helper class to construct window menus.
	"""
	
	mName = str()
	def __init__(self, label, helpMenu = False, parent = False, tearOff = False ):
		"""
		Start a new menu with the gIven options.
		"""
		
		if parent == False:
			self.mName = cmds.menu( label = label, helpMenu = helpMenu, tearOff = tearOff )
		else:
			self.mName = cmds.menu( label = label, helpMenu = helpMenu, tearOff = tearOff, parent = parent )
	
	def getName(self):
		"""
		Return the name of the created menu object
		"""
		
		return self.mName
		
	def addItem(self, label, command, subMenu = False, divider = False, parent = False):
		"""
		Add an item (or submenu) to this menu object.
		"""
		
		if parent == False:
			return cmds.menuItem( label = label, command = command, parent = self.mName, divider = divider, subMenu = subMenu)
		else:
			return cmds.menuItem( label = label, command = command, divider = divider, subMenu = subMenu, parent = parent)
		
	def end(self):
		"""
		End this menu object. No more items can be added.
		"""
		
		cmds.setParent( '..', menu = True)
		
class mOptionMenu(mMenu):
	"""
	GUI helper class to construct optionMenus
	"""
	
	data = 0
	def __init__(self, parent, changeCommand = ''):
		"""
		Start a new optionMenu
		"""
		
		if changeCommand == '':
			self.mName = cmds.optionMenu( height = 24, parent = parent )
		else:
			self.mName = cmds.optionMenu( height = 24, parent = parent, changeCommand = changeCommand)
		
	def addItem(self, label):
		"""
		Add an item to this optionMenu
		"""
		
		itemName = cmds.menuItem( label = label, data = self.data, parent = self.mName)
		self.data += 1

class Girl_gui(OpenMayaMPx.MPxCommand):
	"""
	The Girl Exporter GUI Main class. This is huge, and could possibly be split up.
	"""

	VERSION = '0.5.1'

	Girl_GUI_height = 550	   # Maya adds 20px to this
	Girl_GUI_width  = 400	   # this is the width of a column.
	Girl_GUI_thirdWidth = 120
	Girl_GUI_2thirdWidth = 240
	Girl_GUI_scrollbar_width = 0 # no scrollbars anymore

	@staticmethod
	def commandName():
		return "Girl_gui"

	@staticmethod
	def commandCreator():
		return OpenMayaMPx.asMPxPtr( Girl_gui() )

#	def __init__(self):
#		pass
		
	def makeMainMenu(self):
		"""
		Make the main Girl menu which resides in the Rendering menuSet
		"""
		
		gMainWindow = mel.eval('$tmpVar=$gMainWindow')
		
		GirlMenu = mMenu(label = 'Girl', parent = gMainWindow, tearOff = True )
		GirlMenu.addItem( label = 'Show GUI', command = self.doIt )
		GirlMenu.addItem( label = "Export" , command = self.goBatch )
		GirlMenu.addItem( label = '', command = '', divider = True )
		GirlCreateMenu = GirlMenu.addItem( label = "Create", command = '', subMenu = True )
		GirlMenu.addItem( label = 'PLY Object Locator', command = self.mnuObjectLocator, parent = GirlCreateMenu )
		GirlMenu.addItem( label = 'Environment Light', command = self.mnuEnvLight, parent = GirlCreateMenu )
		GirlMenu.addItem( label = 'Sun + Sky', command = self.mnuSunsky, parent = GirlCreateMenu )
		GirlMenu.end()

		renderMS = mel.eval('findMenuSetFromLabel("Rendering")')
		cmds.menuSet( currentMenuSet = renderMS )
		menuIndex  = cmds.menuSet( query = True, numberOfMenus = True)
		cmds.menuSet( insertMenu = (GirlMenu.getName(), menuIndex) )
		GirlMenu.end()

	# GUI Hierarchy construction methods
		
	def newFrame(self, label, parent = False, collapsable = False, collapsed = False):
		"""
		Start a new frameLayout (and transparently add a columnLayout)
		"""
		
		if parent == False:
			theFrame = cmds.frameLayout( label = label,
										 collapsable = collapsable,
										 collapse = collapsed,
										 labelAlign = "top",
										 borderStyle = "etchedOut" )
		else:
			theFrame = cmds.frameLayout( parent = parent,
										 label = label,
										 collapsable = collapsable,
										 collapse = collapsed,
										 labelAlign = "top",
										 borderStyle = "etchedOut" )
			
		# LEVEL 3
		return cmds.columnLayout( rowSpacing = 3,
								  columnAttach = ("both", 2),
								  columnWidth = self.Girl_GUI_width,
								  parent = theFrame ), theFrame
	
	def endFrame(self):
		"""
		End the frameLayout (and the transparent columnLayout)
		"""
		
		cmds.setParent( '..', upLevel = True )
		cmds.setParent( '..', upLevel = True )
		
	def newRow(self, parent, numberOfColumns = 2, w1 = 0, w2 = 0):
		"""
		Start a new rowLayout
		"""
		
		if w1 == 0: w1 = self.Girl_GUI_thirdWidth
		if w2 == 0: w2 = self.Girl_GUI_2thirdWidth
		if numberOfColumns == 2:
			return cmds.rowLayout( numberOfColumns = numberOfColumns,
								   columnWidth2 = (w1, w2),
								   parent = parent )
		else:
			return cmds.rowLayout( numberOfColumns = numberOfColumns,
								   columnWidth1 = self.Girl_GUI_2thirdWidth,
								   parent = parent )
	
	def endRow(self):
		"""
		End a rowLayout
		"""
		
		cmds.setParent( '..', upLevel = True )
		
	def newText(self, label, parent, addColon = True, width = False):
		"""
		Insert a text control (ie, print text on the GUI)
		"""
		
		if addColon:
			lString = '%s:'
		else:
			lString = '%s'
			
		if not width == False:
			cmds.text( label = lString % label,
					   parent = parent,
					   width = width )
		else:
			cmds.text( label = lString % label,
					   parent = parent )
		
	def startLevel(self):
		"""
		Start a GUI sub-level (adds padding around controls)
		"""
		
		return cmds.columnLayout( rowSpacing = 3,
								  columnAttach = ("left", 4) )
		
	def endLevel(self):
		"""
		End a GUI sub-level
		"""
		
		self.endRow()
		
	def addTextField(self, parent ):
		"""
		Add an editble text field to the GUI
		"""
		
		return  cmds.textField( width = self.Girl_GUI_2thirdWidth,
								parent = parent )

	def addCheckBox(self, parent, label = '', value = False):
		"""
		Add a labelled checkbox to the GUI 
		"""
		
		return cmds.checkBox( parent = parent,
							  label = label,
							  value = value,
							  height = 12)

	def addIntField(self, parent, min, max, value):
		"""
		Add an integer input field to the GUI (not yet implemented)
		"""
		
		pass
	
	def addFloatField(self, parent, min, max, value, changeCommand = False):
		"""
		Add a float input field to the GUI
		"""
		
		if not changeCommand:
			return cmds.floatField( parent = parent, min = min, max = max, value = value )
		else:
			return cmds.floatField( parent = parent, min = min, max = max, value = value, changeCommand = changeCommand )

	def makeTieleTabs(self, inputDict):
		"""
		Make a tabLayout set from the given dict of { controlGroup: 'tab label' }
		"""
		
		tForm = cmds.formLayout()
		tTabs = cmds.tabLayout( parent = tForm, innerMarginWidth = 3, innerMarginHeight = 3 )
		cmds.formLayout( tForm, edit = True,  attachForm=((tTabs, 'top', 40), (tTabs, 'left', 0), (tTabs, 'bottom', 0), (tTabs, 'right', 0)) )
		
		tabLabels = []
		for controlGrp in inputDict:
			cGrp = controlGrp( parent = tTabs )
			tabLabels.append( (cGrp, inputDict[controlGrp]) )
		
		cmds.tabLayout( tTabs, edit = True, tabLabel = tabLabels )

	def makeTabs(self, inputDict):
		"""
		Make a tabLayout set from the given dict of { controlGroup: 'tab label' }
		"""
		
		tForm = cmds.formLayout()
		tTabs = cmds.tabLayout( parent = tForm, innerMarginWidth = 3, innerMarginHeight = 3 )
		cmds.formLayout( tForm, edit = True,  attachForm=((tTabs, 'top', 0), (tTabs, 'left', 0), (tTabs, 'bottom', 0), (tTabs, 'right', 0)) )
		
		tabLabels = []
		for controlGrp in inputDict:
			cGrp = controlGrp( parent = tTabs )
			tabLabels.append( (cGrp, inputDict[controlGrp]) )
		
		cmds.tabLayout( tTabs, edit = True, tabLabel = tabLabels )

	def doIt(self, args = OpenMaya.MArgList() ):
		"""
		Start constructing the GUI, starting with the window itself, and then
		all child control groups.
		"""
		
		ppath = cmds.internalVar( userPrefDir = True )
		ppath = ppath.replace('prefs','presets/images')
		ppath = ppath + "GirlLogo.jpg"
		#cmdName = "image -image \""+ppath+"\"  -height  33;"
		    

		# Check for script node and create/upgrade as necessary
		ls = Girl_settings()
		ls.doIt(OpenMaya.MArgList())
		"""

		# Check for existing window and remove it
		if cmds.window('GirlGuiMain', exists=True):
			cmds.deleteUI('GirlGuiMain', window=True)
			
		if OpenMaya.MGlobal.mayaState() != OpenMaya.MGlobal.kInteractive:
			OpenMaya.MGlobal.displayInfo('GirlMaya: Running in console, not building GUI')
			return
		
		# Create window
		cmds.window('GirlGuiMain', title="Girl Render Exporter",
                                  height = self.Girl_GUI_height,
                                  width = ((self.Girl_GUI_width) + self.Girl_GUI_scrollbar_width),
                                  sizeable = True,
                                  resizeToFitChildren = True,
                                  menuBar = True,
                                  minimizeButton = True)

		self.makeGUIMenus()

		#---
		if os.path.exists(ppath):
                        tForm = cmds.formLayout()
			tLow = cmds.rowLayout( numberOfColumns = 2,
					       columnWidth2 = (308, 100),
					       parent = tForm )

                        cmds.image( parent = tLow, image=ppath, height=40, width=284 )

                        # LEVEL 2 Add button and progress bar
                        exportButton  = cmds.button(height = 40,
                                             label = "Export Scene",
                                             align = "center",
                                             parent = tLow,
                                             command = self.goBatch )

                        cmds.setParent( '..', upLevel = True )
                        cmds.setParent( '..', upLevel = True )


		#---

                self.makeTieleTabs({
                                self.fileFrame: 'Scene',
                                self.processFrame: 'Process',
                                self.networkFrame: 'Multi-threaded rendering',
                                self.filmFrame: 'Film',
                                self.cameraFrame: 'Camera',
                                self.renderFrame: 'Renderer',
                                self.shaderFrame: 'Shader translation'
                             })
		self.endRow() # end the tabs
                        
                # finally, show the window!
		cmds.showWindow( 'GirlGuiMain' )
		"""
		
	def makeGUIMenus(self):
		"""
		Make the menus on the GUI window
		"""
		
		# Create menus
		fileMenu = mMenu( label = "File" )
		fileMenu.addItem( label = 'Export', command = self.goBatch )
		fileMenu.addItem( label = '', command = '', divider = True )
		fileMenu.addItem( label = "Close", command = self.mnuClose )
		fileMenu.end()
		
		createMenu = mMenu( label = 'Create' )
		createMenu.addItem( label = 'PLY Object Locator' , command = self.mnuObjectLocator )
		createMenu.addItem( label = 'Environment Light', command = self.mnuEnvLight )
		createMenu.addItem( label = 'Sun + Sky', command = self.mnuSunsky )
		createMenu.end()
		
		settingsMenu = mMenu( label = 'Settings' )
		settingsMenu.addItem( label = "Maya Preferences",  command = self.mnuPrefs )
		settingsMenu.addItem( label = "Show Girl settings", command = self.mnuGirlSettings )
		settingsMenu.addItem( label = '', command = '', divider = True )
		settingsMenu.addItem( label = "Restore default paths", command = Girl_settings.setScriptNodeDefaultPaths )
		settingsMenu.addItem( label = '', command = '', divider = True )
		settingsRSMenu = settingsMenu.addItem( label = 'Render Presets', command = '', subMenu = True )
		
		presetList = fn_attr.findAttrPresets( 'Girl_settings' )
		presetList.sort()
		for preset in presetList:
			if preset[0:4] == "Girl":
				niceName = preset.replace('Girl_','')
				niceName = niceName.replace('_',' ')
				niceName = niceName.replace('-',' ')
				settingsMenu.addItem( label = niceName, command = "from Girl.GirlMiscModules.fn_attr import fn_attr\nfn_attr.applyAttrPreset( 'Girl_settings', '%s', 100)" % preset, parent = settingsRSMenu)
		settingsMenu.end()
		
		helpMenu = mMenu( label = 'Help', helpMenu = True )
		helpMenu.addItem( label = 'GirlRender website', command = self.mnuHelpWebsite )
		helpMenu.addItem( label = 'Documentation Wiki', command = self.mnuHelpWiki )
		helpMenu.addItem( label = 'Development Forum', command = self.mnuHelpDevForum )
		helpMenu.addItem( label = '', command = '', divider = True )
		helpMenu.addItem( label = 'About', command = self.mnuHelpAbout )
		helpMenu.end()
		

	
	# GUI Frames start here.
	# We can't indent the python code to show the GUI hierarchy, so instead
	# I'm using comment markers, like: #--
		
	def fileFrame(self, parent):
		"""
		Make the frame for the 'Scene' window tab
		"""
		
		#-
		# LEVEL 2 FILE PATH SETTINGS
		fileFrame, fileFrameContainer = self.newFrame( label = "Scene settings", parent = parent )
		#--
		fileRow1 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = "Path to Girl", parent = fileRow1 )
		Girl_file_Girlpath = self.addTextField( parent = fileRow1 )
		cmds.connectControl( Girl_file_Girlpath, 'Girl_settings.Girl_path' )
		#---
		self.endRow()
		fileRow2 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = "Scene export path", parent = fileRow2 )
		Girl_file_scenepath = self.addTextField( parent = fileRow2 )
                #cmds.setAttr( 'Girl_settings.scene_path', '%sGirl%s' % ( cmds.workspace( q = True, rootDirectory = True), os.altsep ), type='string' )
		gorpath = os.getenv('GoRillaPath')
	        cmds.setAttr( 'Girl_settings.scene_path', '%s\\cache' % gorpath, type='string' )



		cmds.connectControl( Girl_file_scenepath, 'Girl_settings.scene_path' )
		#---
		self.endRow()
		fileRow3 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = "Scene name", parent = fileRow3 )
		Girl_file_scenename = self.addTextField( parent = fileRow3 )
		cmds.connectControl( Girl_file_scenename, 'Girl_settings.scene_filename' )
		#---
		self.endRow()
		#--
		fileRow5 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = 'Export' , parent = fileRow5 )
		fileExportRow = self.startLevel()
		#----
		matExportRow = self.newRow( parent = fileExportRow, numberOfColumns = 1)
		#-----
		Girl_file_export_mats = self.addCheckBox( parent = matExportRow, label = 'Materials', value = True )
		cmds.connectControl( Girl_file_export_mats, 'Girl_settings.scene_export_materials' )
		#-----
		self.endRow()
		#----
		meshExportRow = self.newRow( parent = fileExportRow, numberOfColumns = 1)
		#-----
		Girl_file_export_meshes = self.addCheckBox( parent = meshExportRow, label = 'Polygon Meshes', value = True )
		cmds.connectControl( Girl_file_export_meshes, 'Girl_settings.scene_export_meshes' )
		#-----
		self.endRow()
		#----
		nurbsExportRow = self.newRow( parent = fileExportRow, numberOfColumns = 1)
		#-----
		Girl_file_export_nurbs = self.addCheckBox( parent = nurbsExportRow, label = 'NURBS Surfaces', value = False )
		cmds.connectControl( Girl_file_export_nurbs, 'Girl_settings.scene_export_nurbs' )
		#-----
		self.endRow()
		#-----
		subdivExportRow = self.newRow( parent = fileExportRow, numberOfColumns = 1)
		#-----
		Girl_file_export_subdiv = self.addCheckBox( parent = subdivExportRow, label = 'Subdiv Surfaces', value = False )
		cmds.connectControl( Girl_file_export_subdiv, 'Girl_settings.scene_export_subdivs' )
		#-----
		self.endRow()
		#----
		subdivExportLevelRow = self.newRow( parent = fileExportRow)
		#-----
		self.newText( label = 'Subdiv Levels', parent = subdivExportLevelRow )
		Girl_file_export_subdiv_level = cmds.intField( parent = subdivExportLevelRow, min = 1, max = 5, value = 1)
		cmds.connectControl( Girl_file_export_subdiv_level, 'Girl_settings.scene_export_subdivs_levels' )
		#-----
		self.endRow()
		#----
		volsExportRow = self.newRow( parent = fileExportRow, numberOfColumns = 1)
		#-----
		Girl_file_export_vols = self.addCheckBox( parent = volsExportRow, label = 'Fluid Volumes', value = False )
		cmds.connectControl( Girl_file_export_vols, 'Girl_settings.scene_export_volumes' )
		#-----
		self.endRow()
		#----
		self.endLevel()
		#---
		self.endRow()
		#--
		fileRow4 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = 'Collect files', parent = fileRow4 )
		fileCollectRow = self.startLevel()
		#----
		texCollectRow = self.newRow( parent = fileCollectRow, numberOfColumns = 1)
		#-----
		Girl_file_collect_tex = self.addCheckBox( parent = texCollectRow, label = 'Texture maps', value = False)
		cmds.connectControl( Girl_file_collect_tex, 'Girl_settings.scene_collect_texture' )
		#-----
		self.endRow() # texCollectRow
		#----
		bumpCollectRow = self.newRow( parent = fileCollectRow, numberOfColumns = 1)
		#-----
		Girl_file_collect_bump = self.addCheckBox( parent = bumpCollectRow, label = 'Bump maps', value = False)
		cmds.connectControl( Girl_file_collect_bump, 'Girl_settings.scene_collect_bump' )
		#-----
		self.endRow() # bumpCollectRow
		#----
		hdriCollectRow = self.newRow( parent = fileCollectRow, numberOfColumns = 1)
		#-----
		Girl_file_collect_hdri = self.addCheckBox( parent = hdriCollectRow, label = 'HDRI Enviroment maps', value = False)
		cmds.connectControl( Girl_file_collect_hdri, 'Girl_settings.scene_collect_hdri' )
		#-----
		self.endRow() # hdriCollectRow
		#----
		self.endLevel() # fileCollectRow
		#---
		self.endRow() # fileRow4
		#--
		
		fileRow6 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = 'Animation Options', parent = fileRow6 )
		fileAnimLevel = self.startLevel()
		#----
		Girl_file_anim_row1 = self.newRow( parent = fileAnimLevel, numberOfColumns = 1 )
		#-----
		Girl_file_animation = self.addCheckBox( parent = Girl_file_anim_row1, label = 'Export animation', value = False)
		cmds.connectControl( Girl_file_animation, 'Girl_settings.render_animation' )
		#-----
		self.endRow()
		#----
		Girl_file_anim_row2 = self.newRow( parent = fileAnimLevel, numberOfColumns = 1 )
		#-----
		Girl_file_animation_seq = self.addCheckBox( parent = Girl_file_anim_row2, label = 'Export/Render in sequence', value = False)
		cmds.connectControl( Girl_file_animation_seq, 'Girl_settings.render_animation_sequence' )
		#-----
		self.endRow()
		#----
		self.endLevel()
		#---
		self.endRow()
		#--
		
		fileRow7 = self.newRow( parent = fileFrame )
		#---
		self.newText( label = 'Color Controls' , parent = fileRow7 )
		fileColorRow = self.startLevel()
		#----
		rgcRow = self.newRow( parent = fileColorRow, numberOfColumns = 1)
		#-----
		Girl_file_rgc = self.addCheckBox( parent = rgcRow, label = 'Reverse Gamma Correction', value = True )
		cmds.connectControl( Girl_file_rgc, 'Girl_settings.scene_reverse_gamma' )
		#-----
		self.endRow()
		#----
		clampRow = self.newRow( parent = fileColorRow, numberOfColumns = 1)
		#-----
		Girl_file_clamp = self.addCheckBox( parent = clampRow, label = 'Clamp: 0 <= c <= 0.9', value = True )
		cmds.connectControl( Girl_file_clamp, 'Girl_settings.scene_clamp_color' )
		#-----
		self.endRow()
		#----
		self.endLevel()
		#---
		self.endRow()
		#--
		
		
		self.endFrame()
		#-
		
		return fileFrameContainer
		
	def shaderFrame(self, parent):
		"""
		Make the frame for the 'Shader' window tab
		"""
		
		#-
		# LEVEL 3 SHADER SETTINGS
		shaderFrame, shaderFrameContainer = self.newFrame( label = 'Shader seting', parent = parent )

		shader_row1 = self.newRow( parent = shaderFrame, numberOfColumns = 1 )

		Girl_shader_auto_translation = self.addCheckBox( parent = shader_row1, label = 'Shader auto translation', value = False )
		cmds.connectControl( Girl_shader_auto_translation, 'Girl_settings.shader_auto_translation' )

		self.endRow()

		#--
		self.endFrame()
		#-
		
		return shaderFrameContainer

	def processFrame(self, parent):
		"""
		Make the frame for the 'Process' window tab
		"""
		
		#-
		# LEVEL 2 PROCESS SETTINGS
		processFrame, processFrameContainer = self.newFrame( label = 'Process settings', parent = parent )
		#--
		process_row1 = self.newRow( parent = processFrame )
		#---
		self.newText( label = '' , parent = process_row1 )
		renderLaunchLevel = self.startLevel()
		Girl_process_launch = self.addCheckBox( parent = renderLaunchLevel, label = 'Render after export' )
		cmds.connectControl( Girl_process_launch, 'Girl_settings.render_launch' )
		self.endLevel()
		#---
		self.endRow()
		#--
		process_row2 = self.newRow( parent = processFrame )
		#---
		self.newText( label = 'Render interface' , parent = process_row2 )
		Girl_process_interface = mOptionMenu( parent = process_row2 )
		Girl_process_interface.addItem( label = "GUI" )
		Girl_process_interface.addItem( label = "Console" )
		Girl_process_interface.end()
		cmds.connectControl( Girl_process_interface.getName(), 'Girl_settings.render_interface' )
		#---
		self.endRow()
		#--
		process_row3 = self.newRow( parent = processFrame )
		#---
		self.newText( label = 'Render threads' , parent = process_row3 )
		Girl_render_threads = cmds.intField( parent = process_row3, min = 1, max = 64, value = 1)
		cmds.connectControl( Girl_render_threads, 'Girl_settings.render_threads' )
		#---
		self.endRow()
		#--
		process_row4 = self.newRow( parent = processFrame )
		#---
		self.newText( label = 'Render priority' , parent = process_row4 )
		Girl_process_priority = mOptionMenu( parent = process_row4 )
		Girl_process_priority.addItem( label = "Realtime" )
		Girl_process_priority.addItem( label = "High" )
		Girl_process_priority.addItem( label = "Above Normal" )
		Girl_process_priority.addItem( label = "Normal" )
		Girl_process_priority.addItem( label = "Below Normal" )
		Girl_process_priority.addItem( label = "Low" )
		Girl_process_priority.end()
		cmds.connectControl( Girl_process_priority.getName(), 'Girl_settings.render_priority' )
		#---
		self.endRow()
		#--
		self.endFrame()

		
		return processFrameContainer
		
	def networkFrame(self, parent):
		"""
		Make the frame for the 'Network' window tab
		"""
		
		#-
		# LEVEL 3 NETWORK SETTINGS
		networkFrame, networkFrameContainer = self.newFrame( label = 'Network settings', parent = parent )

		network_row1 = self.newRow( parent = networkFrame, numberOfColumns = 2, w1 = 150, w2 = 50 )
		#--
		self.newText( label = '', parent = network_row1 )
		Girl_render_network = self.addCheckBox( parent =network_row1, label = 'Network Render' )
		cmds.connectControl( Girl_render_network, 'Girl_settings.render_network' )
		#--
		self.endRow()
		#-
		
		network_row2 = self.newRow( parent = networkFrame, numberOfColumns = 2, w1 = 150, w2 = 50  )
		#--
		self.newText( label = 'Server request interval', parent = network_row2 )
		Girl_render_network_interval = cmds.intField( parent = network_row2, min = 1, value = 180 )
		cmds.connectControl( Girl_render_network_interval, 'Girl_settings.render_network_interval' )
		#--
		self.endRow()
		#-
		
		network_row3 = self.newRow( parent = networkFrame, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#--
		self.newText( label = 'Network servers', parent = network_row3 )
		Girl_render_network_servers = self.addTextField( parent = network_row3 )
		cmds.connectControl( Girl_render_network_servers, 'Girl_settings.render_network_servers' )
		#--
		self.endRow()
		#-
		#--
		self.endFrame()
		#-
		
		return networkFrameContainer

	def cameraFrame(self, parent):
		"""
		Make the frame for the 'Camera' window tab
		"""
		
		#-
		# LEVEL 2 CAMERA SETTINGS
		cameraFrame, cameraFrameContainer = self.newFrame( label = 'Camera settings', parent = parent )
		#--
		camera_row1 = self.newRow( parent = cameraFrame, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#---
		self.newText( label = 'Perspective camera type', parent = camera_row1 )
		Girl_camera_persptype = mOptionMenu( parent = camera_row1 )
		Girl_camera_persptype.addItem( label = 'Perspective' )
		Girl_camera_persptype.addItem( label = 'Environment' )
		# Girl_camera_persptype.addItem( label = 'Realistic' )
		Girl_camera_persptype.end()
		cmds.connectControl( Girl_camera_persptype.getName(), 'Girl_settings.camera_persptype' )
		#---
		self.endRow()
		#--
		camera_row2 = self.newRow( parent = cameraFrame, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#---
		self.newText( label = 'Camara postion', parent = camera_row2 )
		Girl_camera_name = mOptionMenu( parent = camera_row2 )
		Girl_camera_name.addItem( label = "now-postion" )
		camsList = []
		for camsList in cmds.listCameras():
			if ( camsList != 'front' and camsList != 'persp' and camsList != 'side' and camsList != 'top' ):
				Girl_camera_name.addItem( label = camsList )
		Girl_camera_name.end()
		cmds.connectControl( Girl_camera_name.getName(), 'Girl_settings.camera_name' )
		#---
		self.endRow()

		#--
		cameraFakeLevel = self.startLevel() # this is purely aesthetic
		camera_row3 = self.newRow( parent = cameraFakeLevel, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#---
		self.newText( label = 'F Stop', parent = camera_row3 )
		Girl_camera_fstop = self.addFloatField(parent = camera_row3, min = 1.0, max = 64.0, value = 5.6 )
		
		cams = []
		for cam in cmds.listCameras():
			cams.append( '%s.fStop' % cam )
			
		cmds.connectControl( Girl_camera_fstop, (cams) )
		
		#---
		self.endRow()
		#--
		camera_row4 = self.newRow( parent = cameraFakeLevel, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#---
		self.newText( label = 'Exposure time', parent = camera_row4 )
		Girl_camera_exposure = self.addFloatField(parent = camera_row4, min = 0.0, max = 64.0, value = 1.0 )
		cmds.connectControl( Girl_camera_exposure, 'Girl_settings.camera_exposuretime' )
		#---
		self.endRow()
		#--
		camera_row5 = self.newRow( parent = cameraFakeLevel, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#---
		self.newText( label = '', parent = camera_row5 )
		Girl_camera_autofocus = self.addCheckBox( parent = camera_row5, label = 'Auto focus', value = False)
		cmds.connectControl( Girl_camera_autofocus, 'Girl_settings.camera_autofocus' )
		#---
		self.endRow()
		
		#--
		camera_row6 = self.newRow( parent = cameraFakeLevel, numberOfColumns = 2, w1 = 150, w2 = 200 )
		#---
		self.newText( label = '', parent = camera_row6 )
		Girl_camera_infinite_focus = self.addCheckBox( parent = camera_row6, label = 'Infinite focus', value = False)
		cmds.connectControl( Girl_camera_infinite_focus, 'Girl_settings.camera_infinite_focus' )
		#---
		self.endRow()
		
		self.endLevel()
		#--
		self.endFrame()
		#-
		
		return cameraFrameContainer
		
	def filmFrame(self, parent):
		"""
		Make the frame for the 'Film' window tab
		"""
		
		#-
		# LEVEL 2 FILM SETTINGS
		filmFrame, filmFrameContainer = self.newFrame( label = "Film settings", parent = parent )
		#--
		film_row1 = self.newRow( parent = filmFrame )
		#---
		self.newText( label = "Film type", parent = film_row1 )
		Girl_film_filmtype = mOptionMenu( parent = film_row1, changeCommand = self.doIt )
		Girl_film_filmtype.addItem( label = "fleximage" )
		Girl_film_filmtype.end()
		cmds.connectControl( Girl_film_filmtype.getName(), 'Girl_settings.film' )
		#---
		self.endRow()
		#--
		
		self.makeTabs( { self.filmFlexImageControls: 'FlexImage' } )
		
                # LEVEL 2 Add button and progress bar
                filmButton  = cmds.button(height = 25,
                             label = "Export Film",
                             align = "center",
                             parent = filmFrame,
                             command = self.goBatchFilm )
                

		#--
		self.endFrame()
		#-
		
		return filmFrameContainer
	
	def filmFlexImageControls(self, parent):
		"""
		Make the frame for the 'Fleximage' film tab
		"""
		
		fleximage_controls, flexImageFrameContainer = self.newFrame(label = 'Fleximage settings', parent = parent)
		
		#---
		film_fleximage_row1 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Reinhard Prescale', parent = film_fleximage_row1)
		Girl_film_fleximage_reinhardprescale = cmds.floatField( parent = film_fleximage_row1, min = 0, max = 30, value = 1.0)
		cmds.connectControl( Girl_film_fleximage_reinhardprescale, 'Girl_settings.film_reinhard_prescale' )
		#----
		self.endRow()
		#---
		film_fleximage_row2 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Reinhard Postscale', parent = film_fleximage_row2)
		Girl_film_fleximage_reinhardpostscale = cmds.floatField( parent = film_fleximage_row2, min = 0, max = 30, value = 1.0)
		cmds.connectControl( Girl_film_fleximage_reinhardpostscale, 'Girl_settings.film_reinhard_postscale' )
		#----
		self.endRow()
		#---
		film_fleximage_row3 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Reinhard Burn', parent = film_fleximage_row3)
		Girl_film_fleximage_reinhardburn = cmds.floatField( parent = film_fleximage_row3, min = 0, max = 30, value = 6.0)
		cmds.connectControl( Girl_film_fleximage_reinhardburn, 'Girl_settings.film_reinhard_burn' )
		#----
		self.endRow()
		#---
		film_fleximage_row4 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Gamma', parent = film_fleximage_row4)
		Girl_film_fleximage_gamma = cmds.floatField( parent = film_fleximage_row4, min = 0, max = 4, value = 2.2)
		cmds.connectControl( Girl_film_fleximage_gamma, 'Girl_settings.film_gamma' )
		#----
		self.endRow()
		#---
		film_fleximage_row5 = self.newRow( parent = fleximage_controls )
		#----
		self.newText( label = 'Pre-multiply Alpha' , parent = film_fleximage_row5 )
		Girl_film_fleximage_premultiply = self.addCheckBox( parent = film_fleximage_row5, value = True )
		cmds.connectControl( Girl_film_fleximage_premultiply, 'Girl_settings.film_premultiplyalpha' )
		#----
		self.endRow()
		#---
		film_fleximage_row6 = self.newRow( parent = fleximage_controls )
		#----
		self.newText( label = 'Write' , parent = film_fleximage_row6 )
		Girl_film_write_controls = self.startLevel()
		#-----
		Girl_film_write_row1 = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_write_tm_exr = self.addCheckBox( parent = Girl_film_write_row1, label = 'Tonemapped EXR', value = True) 
		cmds.connectControl( Girl_film_fleximage_write_tm_exr, 'Girl_settings.film_write_tonemapped_exr' )
		#------
		self.endRow()
		#-----
		Girl_film_write_row2 = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_write_tm_igi = self.addCheckBox( parent = Girl_film_write_row2, label = 'Tonemapped IGI', value = False )
		cmds.connectControl( Girl_film_fleximage_write_tm_igi, 'Girl_settings.film_write_tonemapped_igi' )
		#------
		self.endRow()
		#-----
		Girl_film_write_row3 = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_write_tm_tga = self.addCheckBox( parent = Girl_film_write_row3, label = 'Tonemapped TGA', value = False )
		cmds.connectControl( Girl_film_fleximage_write_tm_tga, 'Girl_settings.film_write_tonemapped_tga' )
		#------
		self.endRow()
		#-----
		Girl_film_write_row4 = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_write_utm_exr = self.addCheckBox( parent = Girl_film_write_row4, label = 'Untonemapped EXR' )
		cmds.connectControl( Girl_film_fleximage_write_utm_exr, 'Girl_settings.film_write_untonemapped_exr' )
		#------
		self.endRow()
		#-----
		Girl_film_write_row5 = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_write_utm_igi = self.addCheckBox( parent = Girl_film_write_row5, label = 'Untonemapped IGI' )
		cmds.connectControl( Girl_film_fleximage_write_utm_igi, 'Girl_settings.film_write_untonemapped_igi' )
		#------
		self.endRow()
		#-----
		Girl_film_write_row6A = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_write_resume = self.addCheckBox( parent = Girl_film_write_row6A, label = 'Resume Film' )
		cmds.connectControl( Girl_film_fleximage_write_resume, 'Girl_settings.film_write_resume_film' )
		#------
		self.endRow()
		#-----
		self.endRow()
		#-----
		Girl_film_write_row6B = self.newRow( parent = Girl_film_write_controls, numberOfColumns = 1 )
		#------
		Girl_film_fleximage_restart_resume = self.addCheckBox( parent = Girl_film_write_row6B, label = 'Restart Resume Film' )
		cmds.connectControl( Girl_film_fleximage_restart_resume, 'Girl_settings.film_restart_resume_film' )
		#------
		self.endRow()
		#-----
		
		self.endLevel()
		#----
		self.endRow()
		#---
		film_fleximage_row7 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Write interval', parent = film_fleximage_row7)
		Girl_film_fleximage_writeinterval = cmds.intField( parent = film_fleximage_row7, min = 0, max = 12000, value = 60)
		cmds.connectControl( Girl_film_fleximage_writeinterval, 'Girl_settings.film_writeinterval' )
		#----
		self.endRow()
		#---
		film_fleximage_row8 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Display interval', parent = film_fleximage_row8)
		Girl_film_fleximage_displayinterval = cmds.intField( parent = film_fleximage_row8, min = 0, max = 12000, value = 12)
		cmds.connectControl( Girl_film_fleximage_displayinterval, 'Girl_settings.film_displayinterval' )
		#----
		self.endRow()
		#---
		film_fleximage_row9 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Reject warmup', parent = film_fleximage_row9)
		Girl_film_fleximage_rejectwarmup = cmds.intField( parent = film_fleximage_row9, min = 0, max = 12000, value = 12)
		cmds.connectControl( Girl_film_fleximage_rejectwarmup, 'Girl_settings.film_reject_warmup' )
		#----
		self.endRow()
		#---
		film_fleximage_row10 = self.newRow( parent = fleximage_controls )
		#----
		self.newText(label = 'Halt SPP', parent = film_fleximage_row10)
		Girl_film_fleximage_haltspp = cmds.intField( parent = film_fleximage_row10, min = 0, value = 0)
		cmds.connectControl( Girl_film_fleximage_haltspp, 'Girl_settings.film_haltspp' )
		#----
		self.endRow()
		#---

		return flexImageFrameContainer

	def renderFrame(self, parent):
		"""
		Make the frame for the 'Renderer' window tab
		"""
		
		#-
		# LEVEL 2 RENDERER SETTINGS
		renderFrame, renderFrameContainer = self.newFrame( label = "Renderer Settings", parent = parent )
		#--
		# SAMPLERS
		SA_FRAME, SA_FRAME_CONTAINER = self.newFrame(label = 'Sampler' , parent = renderFrame, collapsable = True, collapsed = True)
		Girl_render_row1 = self.newRow(parent = SA_FRAME)
		#---
		self.newText(label = "Sampler", parent = Girl_render_row1)
		Girl_render_sampler = mOptionMenu(parent = Girl_render_row1)
		Girl_render_sampler.addItem(label = "Random")
		Girl_render_sampler.addItem(label = "Low discrepancy")
		Girl_render_sampler.addItem(label = "Halton")
		Girl_render_sampler.addItem(label = "Metropolis")
		Girl_render_sampler.addItem(label = "erpt")
		Girl_render_sampler.end()
		cmds.connectControl( Girl_render_sampler.getName(), 'Girl_settings.pixel_sampler' )
		#---
		self.endRow()
		#--
		self.makeTabs({
					   self.samplerRandom: 'Random',
					   self.samplerLowDiscrepancy: 'Low Discrepancy',
					   self.samplerHalton: 'Halton',
					   self.samplerMetropolis: 'Metropolis',
					   self.samplerErpt: 'ERPT'
					   })
		self.endRow()

		
		# FILTERS
		FI_FRAME, FI_FRAME_CONTAINER = self.newFrame(label = 'Filter' , parent = renderFrame, collapsable = True, collapsed = True)
		Girl_render_row2 = self.newRow(parent = FI_FRAME)
		#---
		self.newText(label = "Pixel Filter", parent = Girl_render_row2)
		Girl_render_filter = mOptionMenu(parent = Girl_render_row2)
		Girl_render_filter.addItem(label = "Mitchell")
		Girl_render_filter.addItem(label = "Gaussian")
		Girl_render_filter.addItem(label = "Sinc")
		Girl_render_filter.addItem(label = "Triangle")
		Girl_render_filter.addItem(label = "Box")
		Girl_render_filter.end()
		cmds.connectControl( Girl_render_filter.getName(), 'Girl_settings.pixel_filter' )
		#---
		self.endRow()
		#--
		self.makeTabs({
					   self.filterMitchell: 'Mitchell',
					   self.filterGaussian: 'Gaussian',
					   self.filterSinc: 'Sinc',
					   self.filterTriangle: 'Triangle',
					   self.filterBox: 'Box'
					   })
		self.endRow()
		
		# ACCELERATORS
		AC_FRAME, AC_FRAME_CONTAINER = self.newFrame(label = 'Accelerator', parent = renderFrame, collapsable = True, collapsed = True)
		Girl_render_row3 = self.newRow(parent = AC_FRAME)
		#---
		self.newText(label = "Accelerator", parent = Girl_render_row3)
		Girl_render_accelerator = mOptionMenu(parent = Girl_render_row3)
		Girl_render_accelerator.addItem(label = "KDtreeCache")
                Girl_render_accelerator.addItem(label = "KDtree")
		Girl_render_accelerator.addItem(label = "Grid")
		Girl_render_accelerator.addItem(label = "Unsafe KDtree")
		Girl_render_accelerator.addItem(label = "None")
		Girl_render_accelerator.end()
		cmds.connectControl( Girl_render_accelerator.getName(), 'Girl_settings.accelerator' )
		#---
		self.endRow()
		#--

		self.makeTabs({
                                           self.acceleratorKDtreeCache: 'KD Tree Cache',
                                           self.acceleratorKDtree: 'KD Tree',
					   self.acceleratorGrid: 'Grid',
					   })
		self.endRow()
		
		#--
		
		# SURFACE INTEGRATORS
		SI_FRAME, SI_FRAME_CONTAINER = self.newFrame(label = 'Surface Integrator', parent = renderFrame, collapsable = True, collapsed = True)
		Girl_render_row4 = self.newRow(parent = SI_FRAME)
		#---
		self.newText(label = "Surface Integrator", parent = Girl_render_row4)
		Girl_render_surface_integrator = mOptionMenu(parent = Girl_render_row4)
		Girl_render_surface_integrator.addItem(label = "BiDirectional")
		Girl_render_surface_integrator.addItem(label = "Direct Lighting")
		Girl_render_surface_integrator.addItem(label = "Particle Tracing")
		Girl_render_surface_integrator.addItem(label = "Path")
		Girl_render_surface_integrator.addItem(label = "ExPhotonMap")
		Girl_render_surface_integrator.addItem(label = "Distributed Path")
		Girl_render_surface_integrator.end()
		cmds.connectControl( Girl_render_surface_integrator.getName(), 'Girl_settings.surface_integrator' )
		#---
		self.endRow()
		#--
		
		self.makeTabs({
					   self.renderSurfaceIntegratorBiDir: 'BiDir',
					   self.renderSurfaceIntegratorDirectLighting: 'DirectLighting',
					   self.renderSurfaceIntegratorParticleTracing: 'Particle',
					   self.renderSurfaceIntegratorPath: 'Path',
					   self.renderSurfaceIntegratorExPhotonMap: 'ExPhotonMap',
					   self.renderSurfaceIntegratorDistributedPath: 'DistributedPath'
					   })
		self.endRow()
		
		
		# VOLUME INTEGRATORS
		VI_FRAME, VI_FRAME_CONTAINER = self.newFrame(label = 'Volume Integrator', parent = renderFrame, collapsable = True, collapsed = True)
		Girl_render_row5 = self.newRow(parent = VI_FRAME)
		#---
		self.newText(label = "Volume Integrator", parent = Girl_render_row5)
		Girl_render_volume_integrator = mOptionMenu(parent = Girl_render_row5 )
		Girl_render_volume_integrator.addItem(label = "Emission")
		Girl_render_volume_integrator.addItem(label = "Single")
		Girl_render_volume_integrator.end()
		cmds.connectControl( Girl_render_volume_integrator.getName(), 'Girl_settings.volume_integrator' )
		#---
		self.endRow()
		#--
		self.makeTabs({
					   self.renderVolumeIntegrator: 'Emission && Single'
					   })
		self.endRow()
		
		self.endFrame()
		#-
		return renderFrameContainer
	
	def samplerRandom(self, parent):
		"""
		Make the frame for the 'Sampler' renderer tab
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Random Sampler Settings', parent = parent )
		
		Girl_render_sampler_row1 = self.newRow( parent = fControls )
		#---
		self.newText( label = "xsamples", parent = Girl_render_sampler_row1 )
		Girl_render_sampler_xsamples = cmds.intField( parent = Girl_render_sampler_row1, min = 1, max = 50000, value = 2)
		cmds.connectControl( Girl_render_sampler_xsamples, 'Girl_settings.pixel_sampler_xsamples' )
		#---
		self.endRow()
		#--
		Girl_render_sampler_row2 = self.newRow( parent = fControls )
		#---
		self.newText( label = "ysamples", parent = Girl_render_sampler_row2 )
		Girl_render_sampler_ysamples = cmds.intField( parent = Girl_render_sampler_row2, min = 1, max = 50000, value = 2)
		cmds.connectControl( Girl_render_sampler_ysamples, 'Girl_settings.pixel_sampler_ysamples' )
		#---
		self.endRow()
		#--
		self.samplerCTL_pixelsampler( parent = fControls )
		return fControlContainer
		
	def samplerLowDiscrepancy(self, parent):
		"""
		Make the frame for the 'LowDiscrepancy' sampler tab
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Low Discrepancy Sampler Settings', parent = parent )
		self.samplerCTL_pixelsamples( parent = fControls )
		self.samplerCTL_pixelsampler( parent = fControls )
		return fControlContainer
		
	def samplerHalton(self, parent):
		"""
		Make the frame for the 'Halton' sampler tab
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Halton Sampler Settings', parent = parent )
		self.samplerCTL_pixelsamples( parent = fControls )
		self.samplerCTL_pixelsampler( parent = fControls )
		return fControlContainer
		
	def samplerMetropolis(self, parent):
		"""
		Make the frame for the 'Metropolis' sampler tab
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Metropolis Sampler Settings', parent = parent )
		self.samplerCTL_init_mutation( parent = fControls )
		
		Girl_render_sampler_row3 = self.newRow( parent = fControls )
		#---
		self.newText( label = 'maxconsecrejects', parent = Girl_render_sampler_row3 )
		Girl_render_sampler_maxconsecrejects = cmds.intField( parent = Girl_render_sampler_row3, min = 1, max = 32767, value = 512)
		cmds.connectControl( Girl_render_sampler_maxconsecrejects, 'Girl_settings.pixel_sampler_maxconsecrejects' )
		#---
		self.endRow()
		#--
		Girl_render_sampler_row4 = self.newRow( parent = fControls )
		#---
		self.newText( label = 'largemutationprob', parent = Girl_render_sampler_row4 )
		Girl_render_sampler_largemutationprob = cmds.floatField( parent = Girl_render_sampler_row4, min = 0, max = 1, value = 0.4)
		cmds.connectControl( Girl_render_sampler_largemutationprob, 'Girl_settings.pixel_sampler_largemutationprob' )
		#---
		self.endRow()
		#--
		Girl_render_sampler_row5 = self.newRow( parent = fControls )
		#---
		self.newText( label = 'usevariance', parent = Girl_render_sampler_row5 )
		Girl_render_sampler_usevariance = self.addCheckBox( parent = Girl_render_sampler_row5, label = '' )
		cmds.connectControl( Girl_render_sampler_usevariance, 'Girl_settings.pixel_sampler_usevariance' )
		#---
		self.endRow()
		#--
		return fControlContainer
		
	def samplerErpt(self, parent):
		"""
		Make the frame for the 'ERPT' sampler tab
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'ERPT Sampler Settings', parent = parent )
		self.samplerCTL_init_mutation( parent = fControls )
		
		Girl_render_sampler_row3 = self.newRow( parent = fControls )
		#---
		self.newText( label = 'chainlength', parent = Girl_render_sampler_row3 )
		Girl_render_sampler_chainlength = cmds.intField( parent = Girl_render_sampler_row3, min = 1, max = 32767, value = 2000)
		cmds.connectControl( Girl_render_sampler_chainlength, 'Girl_settings.pixel_sampler_chainlength' )
		#---
		self.endRow()
		#--
		return fControlContainer
		
	def samplerCTL_init_mutation(self, parent):
		"""
		Add the initsamples and mutationrange controls to the parent container
		"""
		
		Girl_render_sampler_row1 = self.newRow( parent = parent )
		#---
		self.newText( label = 'initsamples', parent = Girl_render_sampler_row1 )
		Girl_render_sampler_initsamples = cmds.intField( parent = Girl_render_sampler_row1, min = 1, max = 10000000, value = 100000)
		cmds.connectControl( Girl_render_sampler_initsamples, 'Girl_settings.pixel_sampler_initsamples' )
		#---
		self.endRow()
		#--
		# disabled at request of Radiance
#		Girl_render_sampler_row2 = self.newRow( parent = parent )
#		#---
#		self.newText( label = 'mutationrange', parent = Girl_render_sampler_row2 )
#		Girl_render_sampler_mutationrange = cmds.floatField( parent = Girl_render_sampler_row2, min = 0, max = 10000, value = 45)
#		cmds.connectControl( Girl_render_sampler_mutationrange, 'Girl_settings.pixel_sampler_mutationrange' )
#		#---
#		self.endRow()
#		#--
		
	def samplerCTL_pixelsamples(self, parent):
		"""
		Add the pixelsamples control to the parent container
		"""
		
		Girl_render_sampler_row1 = self.newRow( parent = parent )
		#---
		self.newText( label = 'pixelsamples', parent = Girl_render_sampler_row1 )
		Girl_render_sampler_pixelsamples = cmds.intField( parent = Girl_render_sampler_row1, min = 1, max = 50000, value = 2)
		cmds.connectControl( Girl_render_sampler_pixelsamples, 'Girl_settings.pixel_sampler_pixelsamples' )
		#---
		self.endRow()
		#--

	def samplerCTL_pixelsampler(self, parent):
		"""
		Add the pixelsampler controls to the parent container
		"""
		
		Girl_render_sampler_rowPS = self.newRow( parent = parent )
		#---
		self.newText( label = 'pixelsampler' , parent = Girl_render_sampler_rowPS )
		Girl_render_sampler_pixelsampler_select = mOptionMenu( parent = Girl_render_sampler_rowPS )
		Girl_render_sampler_pixelsampler_select.addItem( label = 'Linear' )
		Girl_render_sampler_pixelsampler_select.addItem( label = 'Vegas' )
		Girl_render_sampler_pixelsampler_select.addItem( label = 'Low Discrepancy' )
		Girl_render_sampler_pixelsampler_select.addItem( label = 'Tile' )
		Girl_render_sampler_pixelsampler_select.addItem( label = 'Random' )
		Girl_render_sampler_pixelsampler_select.addItem( label = 'Hilbert' )
		Girl_render_sampler_pixelsampler_select.end()
		cmds.connectControl( Girl_render_sampler_pixelsampler_select.getName(), 'Girl_settings.pixel_sampler_pixelsampler' )
		#---
		self.endRow()
		#--
	
	def filterMitchell(self, parent):
		"""
		Make the Mitchell filter frame for the filters tabs
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Mitchell Settings', parent = parent )
		self.filterCommonControls( parent = fControls )
		Girl_render_filter_row3 = self.newRow( parent = fControls )
		#---
		self.newText( label = "B", parent = Girl_render_filter_row3 )
		Girl_render_filter_b = cmds.floatField( parent = Girl_render_filter_row3, min = 0, max = 10000, value = 0.333)
		cmds.connectControl( Girl_render_filter_b, 'Girl_settings.pixel_filter_b' )
		#---
		self.endRow()
		#--
		Girl_render_filter_row4 = self.newRow( parent = fControls )
		#---
		self.newText( label = "C", parent = Girl_render_filter_row4 )
		Girl_render_filter_c = cmds.floatField( parent = Girl_render_filter_row4, min = 0, max = 10000, value = 0.333)
		cmds.connectControl( Girl_render_filter_c, 'Girl_settings.pixel_filter_c' )
		#---
		self.endRow()
		#--
		return fControlContainer

	def filterGaussian(self, parent):
		"""
		Make the Gaussian filter frame for the filters tabs
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Gaussian Settings', parent = parent )
		self.filterCommonControls( parent = fControls )
		
		Girl_render_filter_row3 = self.newRow( parent = fControls )
		#---
		self.newText( label = "alpha", parent = Girl_render_filter_row3 )
		Girl_render_filter_alpha = cmds.floatField( parent = Girl_render_filter_row3, min = 0, max = 10000, value = 2)
		cmds.connectControl( Girl_render_filter_alpha, 'Girl_settings.pixel_filter_alpha' )
		#---
		self.endRow()
		#--
		return fControlContainer
		
	def filterSinc(self, parent):
		"""
		Make the Sinc filter frame for the filters tabs
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Sinc Settings', parent = parent )
		self.filterCommonControls( parent = fControls )
		
		Girl_render_filter_row3 = self.newRow( parent = fControls )
		#---
		self.newText( label = "tau", parent = Girl_render_filter_row3 )
		Girl_render_filter_tau = cmds.floatField( parent = Girl_render_filter_row3, min = 0, max = 10000, value = 3)
		cmds.connectControl( Girl_render_filter_tau, 'Girl_settings.pixel_filter_tau' )
		#---
		self.endRow()
		#--
		return fControlContainer
		
	def filterTriangle(self, parent):
		"""
		Make the Triangle filter frame for the filters tabs
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Triangle Settings', parent = parent )
		self.filterCommonControls( parent = fControls )
		return fControlContainer
		
	def filterBox(self, parent):
		"""
		Make the Box filter frame for the filters tabs
		"""
		
		fControls, fControlContainer = self.newFrame( label = 'Box Settings', parent = parent )
		self.filterCommonControls( parent = fControls )
		return fControlContainer
		
	def filterCommonControls(self, parent):
		"""
		Make the Common filter controls for the filters tabs
		"""
		
		Girl_render_filter_row1 = self.newRow( parent = parent )
		#---
		self.newText( label = "xwidth", parent = Girl_render_filter_row1 )
		Girl_render_filter_xwidth = cmds.floatField( parent = Girl_render_filter_row1, min = 0, max = 10000, value = 2)
		cmds.connectControl( Girl_render_filter_xwidth, 'Girl_settings.pixel_filter_xwidth' )
		#---
		self.endRow()
		#--
		Girl_render_filter_row2 = self.newRow( parent = parent )
		#---
		self.newText( label = "ywidth", parent = Girl_render_filter_row2 )
		Girl_render_filter_ywidth = cmds.floatField( parent = Girl_render_filter_row2, min = 0, max = 10000, value = 2)
		cmds.connectControl( Girl_render_filter_ywidth, 'Girl_settings.pixel_filter_ywidth' )
		#---
		self.endRow()
		#--

	def acceleratorGrid(self, parent):
		"""
		Make the Grid controls for the Accelerator tabs
		"""
		
		acControls, acControlContainer = self.newFrame( label = 'Grid Accelerator Settings', parent = parent )
		
		Girl_render_accelerator_row1 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'refineimmediately' , parent = Girl_render_accelerator_row1 )
		Girl_render_accelerator_refineimmediately = self.addCheckBox( parent = Girl_render_accelerator_row1 )
		cmds.connectControl( Girl_render_accelerator_refineimmediately, 'Girl_settings.accelerator_refineimmediately' )
		#---
		self.endRow()
		#--
	
		return acControlContainer

	def acceleratorKDtreeCache(self, parent):
		"""
		Make the Grid controls for the Accelerator tabs
		"""
		
		acControls, acControlContainer = self.newFrame( label = 'KD Tree Cache Accelerator Settings', parent = parent )
		
		Girl_render_accelerator_row1 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'Cache FileName' , parent = Girl_render_accelerator_row1 )
                cmds.setAttr( 'Girl_settings.accelerator_KDTreeCacheFileName', cmds.getAttr( 'Girl_settings.scene_filename' ) , type='string' )

		Girl_render_accelerator_KDTreeCacheFileName = self.addTextField( parent = Girl_render_accelerator_row1 )
                cmds.connectControl( Girl_render_accelerator_KDTreeCacheFileName, 'Girl_settings.accelerator_KDTreeCacheFileName' )
		#---
		self.endRow()
		#--
		Girl_render_accelerator_row2 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'Force Overwrite' , parent = Girl_render_accelerator_row2 )
		Girl_render_accelerator_KDTreeCacheOverwrite = self.addCheckBox( parent = Girl_render_accelerator_row2 )
                cmds.connectControl( Girl_render_accelerator_KDTreeCacheOverwrite, 'Girl_settings.accelerator_KDTreeCacheOverwrite' )
		#---
		self.endRow()
		#--
	
		return acControlContainer

	def acceleratorUnsafeKDtree(self, parent):
		"""
		Clone the KDTree controls for the Accelerator tabs
		"""
		
		return self.acceleratorKDtree(parent)
		
	def acceleratorKDtree(self, parent):
		"""
		Make the KDTree controls for the Accelerator tabs
		"""
		
		acControls, acControlContainer = self.newFrame( label = 'KD Tree Accelerator Settings', parent = parent )
		
		Girl_render_accelerator_row1 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'intersectcost' , parent = Girl_render_accelerator_row1 )
		Girl_render_accelerator_intersectcost = cmds.intField( parent = Girl_render_accelerator_row1, min = 0, max = 10000, value = 80)
		cmds.connectControl( Girl_render_accelerator_intersectcost, 'Girl_settings.accelerator_intersectcost' )
		#---
		self.endRow()
		#--
		Girl_render_accelerator_row2 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'traversalcost' , parent = Girl_render_accelerator_row2 )
		Girl_render_accelerator_traversalcost = cmds.intField( parent = Girl_render_accelerator_row2, min = 0, max = 10000, value = 1)
		cmds.connectControl( Girl_render_accelerator_traversalcost, 'Girl_settings.accelerator_traversalcost' )
		#---
		self.endRow()
		#--
		Girl_render_accelerator_row3 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'emptybonus' , parent = Girl_render_accelerator_row3 )
		Girl_render_accelerator_emptybonus = cmds.floatField( parent = Girl_render_accelerator_row3, min = 0, max = 10000, value = 0.5)
		cmds.connectControl( Girl_render_accelerator_emptybonus, 'Girl_settings.accelerator_emptybonus' )
		#---
		self.endRow()
		#--
		Girl_render_accelerator_row4 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'maxprims' , parent = Girl_render_accelerator_row4 )
		Girl_render_accelerator_maxprims = cmds.intField( parent = Girl_render_accelerator_row4, min = 0, max = 10000, value = 1)
		cmds.connectControl( Girl_render_accelerator_maxprims, 'Girl_settings.accelerator_maxprims' )
		#---
		self.endRow()
		#--
		Girl_render_accelerator_row5 = self.newRow( parent = acControls )
		#---
		self.newText( label = 'maxdepth' , parent = Girl_render_accelerator_row5 )
		Girl_render_accelerator_maxdepth = cmds.intField( parent = Girl_render_accelerator_row5, min = 0, max = 10000, value = 1)
		cmds.connectControl( Girl_render_accelerator_maxdepth, 'Girl_settings.accelerator_maxdepth' )
		#---
		self.endRow()
		#--
		
		return acControlContainer
	
	def renderVolumeIntegrator(self, parent):
		"""
		Make the Volume Integrator controls for the renderer frame
		"""
		
		viControls, viControlContainer = self.newFrame( label = 'Volume Integrator Settings', parent = parent )
		
		Girl_render_surface_integrator_row1 = self.newRow( parent = viControls )
		#----
		self.newText( label = 'stepsize' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_stepsize = cmds.floatField( parent = Girl_render_surface_integrator_row1, min = 0, max = 10000, value = 1)
		cmds.connectControl( Girl_render_surface_integrator_stepsize, 'Girl_settings.volume_integrator_stepsize' )
		#----
		self.endRow()
		#---
		
		return viControlContainer

	def renderSurfaceIntegratorBiDir(self, parent):
		"""
		Make the BiDir controls for the surface integrators tabs
		"""
		
		siControls, siControlContainer = self.newFrame( label = 'BiDir Surface Integrator Settings', parent = parent )
		
		# rows
		
		Girl_render_surface_integrator_row1 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'eyedepth' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_eyedepth = cmds.intField( parent = Girl_render_surface_integrator_row1, min = 0, max = 10000, value = 8)
		cmds.connectControl( Girl_render_surface_integrator_eyedepth, 'Girl_settings.surface_integrator_eyedepth' )
		#---
		self.endRow()
		#--
		Girl_render_surface_integrator_row2 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'lightdepth' , parent = Girl_render_surface_integrator_row2 )
		Girl_render_surface_integrator_lightdepth = cmds.intField( parent = Girl_render_surface_integrator_row2, min = 0, max = 10000, value = 8)
		cmds.connectControl( Girl_render_surface_integrator_lightdepth, 'Girl_settings.surface_integrator_lightdepth' )
		#---
		self.endRow()
		#--
		
		return siControlContainer
		
	def renderSurfaceIntegratorDirectLighting(self, parent):
		"""
		Make the Direct Lighting controls for the surface integrators tabs
		"""
		
		siControls, siControlContainer = self.newFrame( label = 'Direct Lighting Integrator Settings', parent = parent )
	
		Girl_render_surface_integrator_row1 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'maxdepth' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_maxdepth = cmds.intField( parent = Girl_render_surface_integrator_row1, min = 0, max = 10000, value = 32)
		cmds.connectControl( Girl_render_surface_integrator_maxdepth, 'Girl_settings.surface_integrator_maxdepth' )
		#---
		self.endRow()
		#--
		self.renderSurfaceIntegratorStrategy( siControls )
		
		return siControlContainer
	
	
	def renderSurfaceIntegratorStrategy(self, parent):
		Girl_render_surface_integrator_row2 = self.newRow( parent = parent )
		#---
		self.newText( label = 'strategy' , parent = Girl_render_surface_integrator_row2 )
		Girl_render_surface_integrator_strategy = mOptionMenu(parent = Girl_render_surface_integrator_row2 )
		Girl_render_surface_integrator_strategy.addItem( label = 'All' )
		Girl_render_surface_integrator_strategy.addItem( label = 'One' )
		Girl_render_surface_integrator_strategy.addItem( label = 'Auto' )
		Girl_render_surface_integrator_strategy.end()
		cmds.connectControl( Girl_render_surface_integrator_strategy.getName(), 'Girl_settings.surface_integrator_strategy' )
		#---
		self.endRow()
		#--
		
	def renderSurfaceIntegratorParticleTracing(self, parent):
		"""
		Make the Particle Tracing controls for the surface integrators tabs
		"""
		
		siControls, siControlContainer = self.newFrame( label = 'Particle Tracing Surface Integrator Settings', parent = parent )
		
		Girl_render_surface_integrator_row1 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'maxdepth' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_maxdepth = cmds.intField( parent = Girl_render_surface_integrator_row1, min = 0, max = 10000, value = 32)
		cmds.connectControl( Girl_render_surface_integrator_maxdepth, 'Girl_settings.surface_integrator_maxdepth' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row2 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'rrcontinueprob' , parent = Girl_render_surface_integrator_row2 )
		Girl_render_surface_integrator_rrcontinueprob = cmds.floatField( parent = Girl_render_surface_integrator_row2, min = 0, max = 1, value = 0.65)
		cmds.connectControl( Girl_render_surface_integrator_rrcontinueprob, 'Girl_settings.surface_integrator_rrcontinueprob' )
		#---
		self.endRow()
		#--
		
		return siControlContainer
		
	def renderSurfaceIntegratorPath(self, parent):
		"""
		Make the Path Tracing controls for the surface integrators tabs
		"""
		
		siControls, siControlContainer = self.newFrame( label = 'Path Tracing Surface Integrator Settings', parent = parent )
		
		Girl_render_surface_integrator_row1 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'maxdepth' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_maxdepth = cmds.intField( parent = Girl_render_surface_integrator_row1, min = 0, max = 10000, value = 32)
		cmds.connectControl( Girl_render_surface_integrator_maxdepth, 'Girl_settings.surface_integrator_maxdepth' )
		#---
		self.endRow()
		#--
		self.renderSurfaceIntegratorStrategy( siControls )
		
		return siControlContainer
	
	def renderSurfaceIntegratorExPhotonMap(self, parent):
		"""
		Make the ExPhotonMap controls for the surface integrators tabs
		"""
		
		siControls, siControlContainer = self.newFrame( label = 'ExPhotonMap Surface Integrator Settings', parent = parent )
		
		Girl_render_surface_integrator_row1 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'causticphotons' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_causticphotons = cmds.intField( parent = Girl_render_surface_integrator_row1, min = 0, max = 1000000, value = 20000 )
		cmds.connectControl( Girl_render_surface_integrator_causticphotons, 'Girl_settings.surface_integrator_causticphotons' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row2 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'indirectphotons' , parent = Girl_render_surface_integrator_row2 )
		Girl_render_surface_integrator_indirectphotons = cmds.intField( parent = Girl_render_surface_integrator_row2, min = 0, max = 20000000, value = 100000)
		cmds.connectControl( Girl_render_surface_integrator_indirectphotons, 'Girl_settings.surface_integrator_indirectphotons' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row3 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'nused' , parent = Girl_render_surface_integrator_row3 )
		Girl_render_surface_integrator_nused = cmds.intField( parent = Girl_render_surface_integrator_row3, min = 0, max = 50000, value = 50 )
		cmds.connectControl( Girl_render_surface_integrator_nused, 'Girl_settings.surface_integrator_nused' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row4 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'maxdepth' , parent = Girl_render_surface_integrator_row4 )
		Girl_render_surface_integrator_maxdepth = cmds.intField( parent = Girl_render_surface_integrator_row4, min = 0, max = 10000, value = 5 )
		cmds.connectControl( Girl_render_surface_integrator_maxdepth, 'Girl_settings.surface_integrator_maxdepth' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row5 = self.newRow( parent = siControls )
		#---
		self.newText( label = '' , parent = Girl_render_surface_integrator_row5 )
		Girl_render_surface_integrator_finalgather = self.addCheckBox( parent = Girl_render_surface_integrator_row5, label = 'finalgather', value = True)
		cmds.connectControl( Girl_render_surface_integrator_finalgather, 'Girl_settings.surface_integrator_finalgather' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row6 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'finalgathersamples' , parent = Girl_render_surface_integrator_row6 )
		Girl_render_surface_integrator_finalgathersamples = cmds.intField( parent = Girl_render_surface_integrator_row6, min = 0, max = 10000, value = 32 )
		cmds.connectControl( Girl_render_surface_integrator_finalgathersamples, 'Girl_settings.surface_integrator_finalgathersamples' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row7 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'maxdist' , parent = Girl_render_surface_integrator_row7 )
		Girl_render_surface_integrator_maxdist = self.addFloatField( parent = Girl_render_surface_integrator_row7, min = 0 , max = 20.0, value = 0.1)
		cmds.connectControl( Girl_render_surface_integrator_maxdist, 'Girl_settings.surface_integrator_maxdist' )
		#---
		self.endRow()
		#--
		
		# Reserved for rrthreshold (not used)
#		Girl_render_surface_integrator_row8 = self.newRow( parent = siControls )
#		#---
#		self.newText( label = '' , parent = Girl_render_surface_integrator_row8 )
#		Girl_render_surface_integrator_ = cmds.intField( parent = Girl_render_surface_integrator_row8, min = 0, max = , value = )
#		cmds.connectControl( Girl_render_surface_integrator_, 'Girl_settings.surface_integrator_' )
#		#---
#		self.endRow()
#		#--
		
		Girl_render_surface_integrator_row9 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'gatherangle' , parent = Girl_render_surface_integrator_row9 )
		Girl_render_surface_integrator_gatherangle = self.addFloatField( parent = Girl_render_surface_integrator_row9, min = 0 , max = 360.0, value = 10.0)
		cmds.connectControl( Girl_render_surface_integrator_gatherangle, 'Girl_settings.surface_integrator_gatherangle' )
		#---
		self.endRow()
		#--
		self.renderSurfaceIntegratorStrategy( siControls )
		
		return siControlContainer
	
	def renderSurfaceIntegratorDistributedPath(self, parent):
		
		siControls, siControlContainer = self.newFrame( label = 'DistributedPath Surface Integrator Settings', parent = parent )
		
		Girl_render_surface_integrator_row1 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'Diffuse Depth' , parent = Girl_render_surface_integrator_row1 )
		Girl_render_surface_integrator_diffusedepth = cmds.intField( parent = Girl_render_surface_integrator_row1, min = 0, max = 1000, value = 3 )
		cmds.connectControl( Girl_render_surface_integrator_diffusedepth, 'Girl_settings.surface_integrator_diffusedepth' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row2 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'Glossy Depth' , parent = Girl_render_surface_integrator_row2 )
		Girl_render_surface_integrator_glossydepth = cmds.intField( parent = Girl_render_surface_integrator_row2, min = 0, max = 1000, value = 2 )
		cmds.connectControl( Girl_render_surface_integrator_glossydepth, 'Girl_settings.surface_integrator_glossydepth' )
		#---
		self.endRow()
		#--
		
		Girl_render_surface_integrator_row3 = self.newRow( parent = siControls )
		#---
		self.newText( label = 'Specular Depth' , parent = Girl_render_surface_integrator_row3 )
		Girl_render_surface_integrator_speculardepth = cmds.intField( parent = Girl_render_surface_integrator_row3, min = 0, max = 1000, value = 5 )
		cmds.connectControl( Girl_render_surface_integrator_speculardepth, 'Girl_settings.surface_integrator_speculardepth' )
		#---
		self.endRow()
		#--
		
		
		self.renderSurfaceIntegratorStrategy( siControls )
		
		return siControlContainer


	# GUI Frames end here.
		
	# GUI callback functions follow... 
		
	# MENUS
	
	def mnuObjectLocator(self, *args):
		"""
		Create a new GirlObjectLocator
		"""
		
		lol = OpenMaya.MFnDependencyNode().create( 'GirlObjectLocator' )
	
	def mnuEnvLight(self, *args):
		"""
		Create a new GirlEnvironmentLight
		"""
		
		lel = OpenMaya.MFnDependencyNode().create( 'GirlEnvironmentLightShape' )
		
	def mnuSunsky(self, *args):
		"""
		Create a new GirlSunsky - TODO: detect if already present in scene. Scene should
		only need one.
		"""
		
		lss = OpenMaya.MFnDependencyNode().create( 'GirlSunskyLightShape' )
		
		# this is broken - causes C++ runtime error
		#from Girl.GirlMiscModules.geoSunData import updateSunNode
		#OpenMaya.MNodeMessage.addAttributeChangedCallback(lss, updateSunNode.doIt, lss )
	
	def mnuPrefs(self, *args):
		"""
		Open the Maya Preferences window
		"""
		
		OpenMaya.MGlobal.executeCommand('PreferencesWindow')
		
	def mnuGirlSettings(self, *args):
		"""
		Load the Girl_settings scriptnode into the AttributeEditor
		"""
		
		OpenMaya.MGlobal.executeCommand('updateAE Girl_settings')
		
	def mnuClose(self, *args):
		"""
		Close (delete) the GUI window
		"""
		
		cmds.deleteUI( 'GirlGuiMain' )
		
		
	def mnuHelpWebsite(self, *args):
		if os.name == 'nt':
			os.system('start http://www.Girlrender.net/' )
			 
	def mnuHelpWiki(self, *args):
		if os.name == 'nt':
			os.system('start http://www.Girlrender.net/wiki/index.php/Main_Page' )
		
	def mnuHelpDevForum(self, *args):
		if os.name == 'nt':
			os.system('start http://www.Girlrender.net/forum/viewforum.php?f=28' )
		
	def mnuHelpAbout(self, *args):
		if cmds.window('GirlGuiAbout', exists=True):
			cmds.deleteUI('GirlGuiAbout', window=True)
			
		# Create window
		abWin = cmds.window('GirlGuiAbout', title="Girl Render Exporter",
								  height = 128,
								  width = ((self.Girl_GUI_width) + self.Girl_GUI_scrollbar_width),
								  sizeable = False,
								  resizeToFitChildren = True,
								  menuBar = False,
								  minimizeButton = False)
		
		tForm, tFormContainer = self.newFrame(label='About', parent = abWin, collapsable = False, collapsed = False)
		
		abStrings = [
					 'GirlMaya v%s By ETRI' % self.VERSION,
					 '',
					 'Released under the GPLv3 License',
					 '',
					 'Check the GirlRender website for latest release info and support:',
					 '',
					 'http://www.Girlrender.net/'
					 ]
		
		tRows = {}
		for i, string in enumerate(abStrings):
			tRows[i] = self.newRow(parent = tForm, numberOfColumns = 1)
			self.newText(label = string, parent = tRows[i], addColon = False, width = ((self.Girl_GUI_width) + self.Girl_GUI_scrollbar_width) )
			self.endRow()
		
		self.endFrame()
		cmds.showWindow( 'GirlGuiAbout' )
		
	
	# OTHER COMMANDS
	
	def goBatchFilm(self, *args):
		"""
		Start the export process
		"""
		
		lb = Girl_batch()
		lb.doItFilmExport()

	def goBatch(self, *args):
		"""
		Start the export process
		"""
		
		lb = Girl_batch()
		lb.doIt()
		
	
	# module debug command
	def dprint(self, str):
		if (self.debug):
			print str