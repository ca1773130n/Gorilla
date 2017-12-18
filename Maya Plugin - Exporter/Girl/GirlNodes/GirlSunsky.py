# ------------------------------------------------------------------------------
# Girl sunsky for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: GirlSunsky.py,v 1.2 2008/06/09 13:43:44 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# GirlSunsky: a locator (light) node for Girl's sunsky system
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx
from maya import OpenMayaRender
from maya import OpenMayaUI

from Girl.GirlMiscModules.glRoutines import glRoutines

from GirlNode import GirlNode

class GirlSunsky(OpenMayaMPx.MPxLocatorNode, glRoutines, GirlNode):
	"""
	Sunsky direction locator node for Maya
	"""
	
	# This is the size of the locator on the grid
	displayRadius = 5
	
	@staticmethod
	def nodeName():
		return "GirlSunskyLightShape"

	@staticmethod
	def nodeId():
		return OpenMaya.MTypeId(0x6C757806) # 'Girl' 06

	@staticmethod
	def nodeCreator():
		return OpenMayaMPx.asMPxPtr( GirlSunsky() )

	@staticmethod
	def nodeClassify():
		return "light"

	def __init__(self):
		OpenMayaMPx.MPxLocatorNode.__init__(self)
		
		# init attr objects here
		
		nsamples              = OpenMaya.MObject()
		gain                  = OpenMaya.MObject()
		turbidity             = OpenMaya.MObject()
		relsize               = OpenMaya.MObject()
		
		latitude              = OpenMaya.MObject()
		longitude             = OpenMaya.MObject()
		
		timezone              = OpenMaya.MObject()
		localhour             = OpenMaya.MObject()
		localminute           = OpenMaya.MObject()
		usehourvalue          = OpenMaya.MObject()
		localhourvalue        = OpenMaya.MObject()
		
		day                   = OpenMaya.MObject()
		month                 = OpenMaya.MObject()
		year                  = OpenMaya.MObject()
		usedayvalue           = OpenMaya.MObject()
		dayvalue              = OpenMaya.MObject()
		
#		effectivelocalhour    = OpenMaya.MObject()
#		effectivelocalminute  = OpenMaya.MObject()
#		effectiveday          = OpenMaya.MObject()
#		effectivemonth        = OpenMaya.MObject()
#		
#		elevation             = OpenMaya.MObject()
#		azimuth               = OpenMaya.MObject()

		glRenderer = OpenMayaRender.MHardwareRenderer.theRenderer()
		self.glFT = glRenderer.glFunctionTable()
		
		
		#OpenMaya.MGlobal.displayInfo( "GirlSunsky typeName="+self.typeName()+"\n")
		#OpenMaya.MGlobal.displayInfo( "GirlSunsky name="+self.name()+"\n");

	@staticmethod
	def nodeInitializer():
		enumAttr	= OpenMaya.MFnEnumAttribute()
		nAttr = OpenMaya.MFnNumericAttribute()
		
		try:
			
			GirlSunsky.nsamples = GirlSunsky.makeInteger( 'nsamples', 'ns', default = 1 )
			GirlSunsky.gain = GirlSunsky.makeFloat( 'gain', 'ga', default = 1.0 )
			GirlSunsky.turbidity = GirlSunsky.makeFloat( 'turbidity', 'tb', default = 2.0 )
			GirlSunsky.relsize = GirlSunsky.makeFloat( 'relsize', 'rs', default = 1.0 )
			
			GirlSunsky.latitude = GirlSunsky.makeFloat( 'latitude', 'lat', default = 51.55, input = True )
			GirlSunsky.longitude = GirlSunsky.makeFloat( 'longitude', 'lon', default = -0.12, input = True )
			
			GirlSunsky.timezone = enumAttr.create("timeZone", "tz", 15)
			tzListStr = "-12.00:-11.00:-10.00 US/Hawaii:-9.30:-9.00 CAN/Yukon:-8.30:-8.00 US/Pacific:-7.00 US/Mountain:-6.00 US/Central:-5.00 US/Eastern:-4.00 Caracas:-3.30:-3.00 Rio:-2.0:-1.00:GMT London:+1.00 Paris:+2.00 Cairo:+3.00 Jeddah:+3.30 Tehran:+4.00 Dubai:+4.30 Kabul:+5.00 Karachi:+5.30 Delhi:+6.00 Dhaka:+6.30 Rangoon:+7.00 Bangkok:+8.00 Hong Kong:+9.00 Tokyo:+9.30 Adelaide:+10.00 Sydney:+10.30:+11.00 Noumea:+11.30:+12.00 Wellington:+13.00"
			tzList = tzListStr.split(':')
			i=0
			for tz in tzList:
				enumAttr.addField( tz, i)
				i+=1
			GirlSunsky.makeInput( enumAttr )
			
			GirlSunsky.localhour = GirlSunsky.makeInteger( 'localHour', 'lh', default = 14, input = True )
			GirlSunsky.localminute = GirlSunsky.makeInteger( 'localMinute', 'lm', default = 0, input = True )
			GirlSunsky.usehourvalue = GirlSunsky.makeBoolean( 'useHourValue', 'uhv', input = True )
			GirlSunsky.localhourvalue = GirlSunsky.makeFloat( 'localHourValue', 'lhv', default = 14.0, input = True )
			
			GirlSunsky.day = GirlSunsky.makeInteger( 'day', 'd', default = 25, input = True )
			
			GirlSunsky.month = enumAttr.create("month", "mo", 4)
			mListStr = "Jan:Feb:Mar:Apr:May:Jun:Jul:Aug:Sep:Oct:Nov:Dec"
			mList = mListStr.split(':')
			i=0
			for month in mList:
				enumAttr.addField( month, i )
				i+=1
			GirlSunsky.makeInput( enumAttr )
			
			GirlSunsky.year = GirlSunsky.makeInteger( 'year', 'y', default = 2008, input = True )
			GirlSunsky.usedayvalue = GirlSunsky.makeBoolean( 'useDayValue', 'udv', input = True )
			GirlSunsky.dayvalue = GirlSunsky.makeFloat( 'dayValue', 'dv', default = 144.0, input = True )
			
#			GirlSunsky.effectivelocalhour = GirlSunsky.makeInteger( 'effectiveLocalHour', 'elh', input = True )
#			GirlSunsky.effectivelocalminute = GirlSunsky.makeInteger( 'effectiveLocalMinute', 'elm', input = True )
#			GirlSunsky.effectiveday = GirlSunsky.makeInteger( 'effectiveDay', 'ed', input = True )
#			
#			GirlSunsky.effectivemonth = enumAttr.create("effectiveMonth", "emo", 4)
#			i=0
#			for month in mList:
#				enumAttr.addField( month, i )
#				i+=1
#			GirlSunsky.makeInput( enumAttr )
#			
#			GirlSunsky.elevation = GirlSunsky.makeFloat( 'elevation', 'e', input = True )
#			GirlSunsky.azimuth = GirlSunsky.makeFloat( 'azimuth', 'a', input = True)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to create attributes\n")
			raise
		
		try:
			pass
			# base attributes
			GirlSunsky.addAttribute(GirlSunsky.nsamples)
			GirlSunsky.addAttribute(GirlSunsky.gain)
			GirlSunsky.addAttribute(GirlSunsky.turbidity)
			GirlSunsky.addAttribute(GirlSunsky.relsize)
			
			GirlSunsky.addAttribute(GirlSunsky.latitude)
			GirlSunsky.addAttribute(GirlSunsky.longitude)
			GirlSunsky.addAttribute(GirlSunsky.timezone)
			GirlSunsky.addAttribute(GirlSunsky.localhour)
			GirlSunsky.addAttribute(GirlSunsky.localminute)
			GirlSunsky.addAttribute(GirlSunsky.usehourvalue)
			GirlSunsky.addAttribute(GirlSunsky.localhourvalue)
			GirlSunsky.addAttribute(GirlSunsky.day)
			GirlSunsky.addAttribute(GirlSunsky.month)
			GirlSunsky.addAttribute(GirlSunsky.year)
			GirlSunsky.addAttribute(GirlSunsky.usedayvalue)
			GirlSunsky.addAttribute(GirlSunsky.dayvalue)
#			GirlSunsky.addAttribute(GirlSunsky.effectivelocalhour)
#			GirlSunsky.addAttribute(GirlSunsky.effectivelocalminute)
#			GirlSunsky.addAttribute(GirlSunsky.effectiveday)
#			GirlSunsky.addAttribute(GirlSunsky.effectivemonth)
#			GirlSunsky.addAttribute(GirlSunsky.elevation)
#			GirlSunsky.addAttribute(GirlSunsky.azimuth)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to add attributes\n")
			raise
		
		return OpenMaya.MStatus.kSuccess
	
	def postConstructor(self):
		self._setMPSafe(True)
		
		
	@staticmethod
	def sunskyCallback( msg, plug, otherPlug, srcNode ):
		OpenMaya.MGlobal.displayInfo('Got sunsky callback')
#		from Girl.GirlMiscModules.geoSunData import updateSunNode
#		usn = updateSunNode()
#		args = OpenMaya.MArgList()
#		args.addArg( OpenMaya.MFnDependencyNode( srcNode ).name() )
#		usn.doIt(args)
	
	@staticmethod	
	def isBounded():
		return True
	
	def boundingBox(self):
		bbox = OpenMaya.MBoundingBox()
		bbox.expand( OpenMaya.MPoint( -self.displayRadius, -self.displayRadius, -self.displayRadius ) )
		bbox.expand( OpenMaya.MPoint(  self.displayRadius,  self.displayRadius,  self.displayRadius ) )
		return bbox
	
	
	def draw(self, view, DGpath, style, status):
		
		try:
			col = self.colorRGB( status )
			
			if status == OpenMayaUI.M3dView.kLead:
				self.glFT.glColor4f( 0, 1, 0, 0.3 )
			else:
				self.glFT.glColor4f( col.r, col.g, col.b, 0.3 )
			
			# You want the sun on a stick!!
			self.glFT.glBlendFunc(OpenMayaRender.MGL_SRC_ALPHA,OpenMayaRender.MGL_ONE_MINUS_SRC_ALPHA)
			self.glFT.glEnable(OpenMayaRender.MGL_BLEND)
			self.DrawSphere( self.displayRadius )
			self.glFT.glPushMatrix();
			self.glFT.glTranslatef(0,0,2.0*self.displayRadius);
			self.DrawSphere( self.displayRadius/10.0 )
			self.glFT.glPopMatrix();
			self.glFT.glBegin( OpenMayaRender.MGL_LINES )
			self.glFT.glVertex3f( 0.0,  0.0, 0.0 )
			self.glFT.glVertex3f( 0.0,  0.0, 2.0*self.displayRadius)
			self.glFT.glEnd()
			self.glFT.glDisable(OpenMayaRender.MGL_BLEND)
			
		except:
			OpenMaya.MGlobal.displayError("Failed to draw GirlSunsky\n")
			raise