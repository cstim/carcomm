function csCreateMap() {
    var map = new OpenLayers.Map ("map", {
          controls:
            [
                new OpenLayers.Control.Navigation(),
                new OpenLayers.Control.PanZoomBar(),
                new OpenLayers.Control.Attribution()
                ],
                maxExtent:
            new OpenLayers.Bounds(-20037508.34,-20037508.34,20037508.34,20037508.34),
                maxResolution:
            156543.0399,
                numZoomLevels:
            19,
                units:
            'm',
                projection:
            new OpenLayers.Projection("EPSG:900913"),
                displayProjection:
            new OpenLayers.Projection("EPSG:4326")
                }
        );

    // Define the map layer
    // Other defined layers are OpenLayers.Layer.OSM.Mapnik, OpenLayers.Layer.OSM.Maplint and OpenLayers.Layer.OSM.CycleMap
    var layerMapnik = new OpenLayers.Layer.OSM.Mapnik("Mapnik");
    //layerMapnik.attribution = "";
    if (typeof(on_osm_tileloaded) != "undefined")
        layerMapnik.events.register("tileloaded", layerMapnik, on_osm_tileloaded);
    if (typeof(on_osm_loadstart) != "undefined")
        layerMapnik.events.register("loadstart", layerMapnik, on_osm_loadstart);
    map.addLayer(layerMapnik);

    return map;
}

function csCreateGpxStyleMap() {

    // Create the style for the GPX traces
    var gpxStyle = new OpenLayers.Style(
        { //strokeColor: "green",
          strokeOpacity: 0.5,
                strokeWidth: 10 //'${avgvel}'
                });

    var ruleStopping = new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
                type: OpenLayers.Filter.Comparison.LESS_THAN,
                      property: "avgvel",
                      value: 18 / 3.6
                      }),
                symbolizer: { strokeColor: "red" }
        });
    var ruleCitySlow = new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
                type: OpenLayers.Filter.Comparison.BETWEEN,
                      property: "avgvel",
                      lowerBoundary: 18 / 3.6,
                      upperBoundary: 40 / 3.6
                      }),
                symbolizer: { strokeColor: "yellow" }
        });
    var ruleCityFast = new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
                type: OpenLayers.Filter.Comparison.BETWEEN,
                      property: "avgvel",
                      lowerBoundary: 40 / 3.6,
                      upperBoundary: 65 / 3.6
                      }),
                symbolizer: { strokeColor: "green" }
        });
    var ruleHighwaySlow = new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
                type: OpenLayers.Filter.Comparison.BETWEEN,
                      property: "avgvel",
                      lowerBoundary: 65 / 3.6,
                      upperBoundary: 95 / 3.6
                      }),
                symbolizer: { strokeColor: "cyan" }
        });
    var ruleHighwayFast = new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
                type: OpenLayers.Filter.Comparison.GREATER_THAN,
                      property: "avgvel",
                      value: 95 / 3.6
                      }),
                symbolizer: { strokeColor: "blue" }
        });
    gpxStyle.addRules([ruleStopping,
                       ruleCitySlow,
                       ruleCityFast,
                       ruleHighwaySlow,
                       ruleHighwayFast]);

    var gpxStyleMap = new OpenLayers.StyleMap({'default': gpxStyle});

    return gpxStyleMap;
}

function csCreateLayerGpx(serverUrl, min_lat, max_lat,
                          min_lon, max_lon,
                          min_time, max_time) {
    var layerGpx =
        new OpenLayers.Layer.GML
        ("GML",
         serverUrl + "slices.gpx?"
         + "min_lat=" + min_lat
         + "&max_lat=" + max_lat
         + "&min_lon=" + min_lon
         + "&max_lon=" + max_lon
         + "&min_time=" + min_time
         + "&max_time=" + max_time
         , {
          format:
            OpenLayers.Format.GPX,
                styleMap:
            gpxStyleMap,
                projection:
            new OpenLayers.Projection("EPSG:4326")
        });

//     layerGpx.requestSuccess = function (request) {
//         window.onerror("Yepp, GML worked fine");
//         this.prototype.requestSuccess(request);
//     }

    return layerGpx;
}

function dateToString(date) {
    var zeropad = function (num) { return ((num < 10) ? '0' : '') + num; }
    // 2009-05-01T00:00:00Z
    var timestring = String(date.getFullYear())
        + "-" + zeropad(date.getMonth() + 1)
        + "-" + zeropad(date.getDate())
        + "T" + zeropad(date.getHours())
        + ":" + zeropad(date.getMinutes())
        + ":" + zeropad(date.getSeconds())
        + "Z";
    return timestring;
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
