# NextBus configurator.  Prompts user for transit agency, bus line,
# direction and stop, issues a string which can then be copied & pasted
# into the predictor program.  Not fancy, just uses text prompts,
# minimal error checking.

import urllib
from xml.dom.minidom import parseString

# Open connection, issue request, read & parse XML response ------------------
def req(cmd):
	connection = urllib.urlopen(
	  'http://webservices.nextbus.com'  +
	  '/service/publicXMLFeed?command=' + cmd)
	raw = connection.read()
	connection.close()
	xml = parseString(raw)
	return xml

# Prompt user for a number in a given range ----------------------------------
def getNum(prompt, n):
	while True:
		nb = raw_input('Enter ' + prompt + ' 0-' + str(n-1) + ': ')
		try:                 x = int(nb)
		except ValueError:   continue    # Ignore non-numbers
		if x >= 0 and x < n: return x    # and out-of-range values

# Main application -----------------------------------------------------------

# Get list of transit agencies, prompt user for selection, get agency tag.
dom       = req('agencyList')
elements  = dom.getElementsByTagName('agency')
print 'TRANSIT AGENCIES:'
for i, item in enumerate(elements):
	print str(i) + ') ' + item.getAttribute('title')
n         = getNum('transit agency', len(elements))
agencyTag = elements[n].getAttribute('tag')

# Get list of routes for selected agency, prompt user, get route tag.
dom       = req('routeList&a=' + agencyTag)
elements  = dom.getElementsByTagName('route')
print '\nROUTES:'
for i, item in enumerate(elements):
	print str(i) + ') ' + item.getAttribute('title')
n         = getNum('route', len(elements))
routeTag  = elements[n].getAttribute('tag')

# Get list of directions for selected agency & route, prompt user...
dom       = req('routeConfig&a=' + agencyTag + '&r=' + routeTag)
elements  = dom.getElementsByTagName('direction')
print
print '\nDIRECTIONS:'
for i, item in enumerate(elements):
	print str(i) + ') ' + item.getAttribute('title')
n         = getNum('direction', len(elements))
dirTitle  = elements[n].getAttribute('title') # Save for later
# ...then get list of stop numbers and descriptions -- these are
# nested in different parts of the XML and must be cross-referenced
stopNums  = elements[n].getElementsByTagName('stop')
stopDescs = dom.getElementsByTagName('stop')

# Cross-reference stop numbers and descriptions to provide a readable
# list of available stops for selected agency, route & direction.
# Prompt user for stop number and get corresponding stop tag.
print '\nSTOPS:'
for i, item in enumerate(stopNums):
	stopNumTag = item.getAttribute('tag')
	for d in stopDescs:
		stopDescTag = d.getAttribute('tag')
		if stopNumTag == stopDescTag:
			print str(i) + ') ' + d.getAttribute('title')
			break
n         = getNum('stop', len(stopNums))
stopTag   = stopNums[n].getAttribute('tag')

# The prediction server wants the stop tag, NOT the stop ID, not sure
# what's up with that.

print '\nCOPY/PASTE INTO ARDUINO SKETCH:'
print ('  { 0x70, "' + agencyTag + '", "' + routeTag + '", "' + stopTag +
  '" }, // ' + dirTitle)
