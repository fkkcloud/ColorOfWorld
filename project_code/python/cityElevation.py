'''
Author : Jae Hyun Yoo
Date : Mar 26 2014
Description : getting elevation experimental
'''
#------------------------------------------------
import json
import urllib
from urllib2 import urlopen


#-----------------------------------------------
def drange( min, max, sample):
    ls = []
    min = min * 1000000
    max = max * 1000000
    sample = sample * 1000000
    min = int(min)
    max = int(max)
    sample = int(sample)
    i = min
    while i < max:
        ls.append(i * 0.000001)
        i = i + sample
    return ls
        
#------------------------------------------------
# debug location
latitude = 37.568260
longitude = 126.97783

#earth
latitudes = drange(37.734612, 37.826057, 0.0004)
longitudes = drange(-122.508373, -122.377739, 0.0005)

#debug locations
#latitudes = range(37, 40)
#longitudes = range(126, 129)


#------------------------------------------------
# get Json from location given
def getJson( latitude, longitude, type ):

    urlPath = "http://open.mapquestapi.com/elevation/v1/profile?key=Fmjtd%7Cluur2l082u%2Cbw%3Do5-9a709z&shapeFormat=raw&latLngCollection=" +  str( latitude ) + "," + str( longitude )
    jsonBuffer = urlopen(urlPath)
    jsonObj = json.loads(jsonBuffer.read())
    return jsonObj


#------------------------------------------------
# get city name and count
def getElevation():
    file = open('height_demo_v2', 'w')

    mapData = {}
    for latitude in latitudes:
        for longitude in longitudes:
            bufferJson = getJson( latitude, longitude, type )
            try:
                height = bufferJson['elevationProfile'][0]['height']


            except IndexError:
                print "ERROR INDEX"
                height = 0
            except UnicodeEncodeError:
                print "UNICODE ERROR"
                hegiht = 0
            except :
                print "ERROR"
                height = 0

            # debug print city name

            mapKey = (latitude, longitude)

            mapData[mapKey] = height

            try:
                bufferWrite = str(mapKey[0]) + ',' + str(mapKey[1]) + ',' + str(mapData[mapKey]) + '\n'
                print bufferWrite
                file.write(bufferWrite)
            except:
                bufferWrite = str(mapKey[0]) + ',' + str(mapKey[1]) + ',' + 0 + '\n'
                file.close()
    file.close()
    return mapData

#------------------------------------------------
def main():
    getElevation()

#------------------------------------------------
if __name__ == "__main__":
    main()


