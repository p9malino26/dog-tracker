import Map from 'ol/Map.js';
import OSM, {ATTRIBUTION} from 'ol/source/OSM.js';
import TileLayer from 'ol/layer/Tile.js';
import View from 'ol/View.js';
import {useGeographic} from 'ol/proj.js';
import Point from 'ol/geom/Point.js';
import Feature from 'ol/Feature.js';
import {Circle as CircleStyle, Fill, Style, Stroke} from 'ol/style.js';
import VectorLayer from 'ol/layer/Vector.js';
import VectorSource from 'ol/source/Vector.js';

const defaultPos = [-0.5873,51.2544 ];

useGeographic();

dogMarker = new Feature({
    geometry: new Point(defaultPos),
});

dogMarker.setStyle(newMarkerStyle('green'));

usMarker = new Feature({
    geometry: new Point(defaultPos),
});

usMarker.setStyle(newMarkerStyle('blue'));

const MARKERDICT = {'dog':dogMarker, 'us':usMarker};

const tileLayer = new TileLayer({
  source: new OSM({
    attributions: [
      'Copyright',
      ATTRIBUTION,
    ],
    url:
      'http://192.168.0.10:8080/tile/{z}/{x}/{y}.png',
  }),
});

const markerLayer = new VectorLayer({
    source: new VectorSource({
        features: [dogMarker, usMarker],
    }),
})

const map = new Map({
  layers: [tileLayer, markerLayer],
  target: 'map',
  view: new View({
    center: defaultPos,
    zoom: 19,
    maxZoom:19 
  }),
});


function newMarkerStyle(color) {
    return new Style({
        image: new CircleStyle({
          radius: 7,
          fill: new Fill({color: color}),
          stroke: new Stroke({
            color: 'white',
            width: 2,
          }),
        }),
        
    })
}


top.setMarkerPos = function(markerType, pos) {
    var marker = MARKERDICT[markerType];
    marker.setGeometry(new Point(pos));
}

top.focusMarker= function(markerType) {
    var point = MARKERDICT[markerType].getGeometry().getCoordinates();
    map.getView().setCenter(point);
}

