package com.mapbox.mapboxsdk.testapp.style;

import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.PointF;
import android.support.annotation.RawRes;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.sources.CannotAddSourceException;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Geometry;
import com.mapbox.services.commons.geojson.Point;

import junit.framework.Assert;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.List;

import timber.log.Timber;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

/**
 * Tests for {@link GeoJsonSource}
 */
@RunWith(AndroidJUnit4.class)
public class GeoJsonSourceTests {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }

  @Test
  public void testFeatureCollection() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

        GeoJsonSource source = new GeoJsonSource("source", FeatureCollection.fromJson(readRawResource(rule.getActivity().getResources(), R.raw.test_feature_collection)));
        mapboxMap.addSource(source);

        mapboxMap.addLayer(new CircleLayer("layer", source.getId()));
      }

    });
  }

  @Test
  public void testPointGeometry() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

        GeoJsonSource source = new GeoJsonSource("source", Point.fromCoordinates(new double[] {0d, 0d}));
        mapboxMap.addSource(source);

        mapboxMap.addLayer(new CircleLayer("layer", source.getId()));
      }

    });
  }

  @Test
  public void testFeatureProperties() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

        GeoJsonSource source = new GeoJsonSource("source", readRawResource(rule.getActivity().getResources(), R.raw.test_feature_properties));
        mapboxMap.addSource(source);

        mapboxMap.addLayer(new CircleLayer("layer", source.getId()));
      }

    });
  }

  @Test
  public void testPointFeature() {
    testFeatureFromResource(R.raw.test_point_feature);
  }

  @Test
  public void testLineStringFeature() {
    testFeatureFromResource(R.raw.test_line_string_feature);
  }

  @Test
  public void testPolygonFeature() {
    testFeatureFromResource(R.raw.test_polygon_feature);
  }

  @Test
  public void testMultiPointFeature() {
    testFeatureFromResource(R.raw.test_multi_point_feature);
  }

  @Test
  public void testMultiLineStringFeature() {
    testFeatureFromResource(R.raw.test_multi_line_string_feature);
  }

  @Test
  public void testMultiPolygonFeature() {
    testFeatureFromResource(R.raw.test_multi_polygon_feature);
  }

  protected void testFeatureFromResource(final @RawRes int resource) {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

        GeoJsonSource source = new GeoJsonSource("source");
        mapboxMap.addSource(source);
        Layer layer = new CircleLayer("layer", source.getId());
        mapboxMap.addLayer(layer);

        source.setGeoJson(Feature.fromJson(readRawResource(rule.getActivity().getResources(), resource)));

        mapboxMap.removeLayer(layer);
        mapboxMap.removeSource(source);
      }

    });
  }

  private String readRawResource(Resources resources, @RawRes int rawResource) {
    InputStream is = resources.openRawResource(rawResource);
    Writer writer = new StringWriter();
    char[] buffer = new char[1024];
    try {
      try {
        Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
        int numRead;
        while ((numRead = reader.read(buffer)) != -1) {
          writer.write(buffer, 0, numRead);
        }
      } finally {
        is.close();
      }
    } catch (IOException err) {
      fail(err.getMessage());
    }

    return writer.toString();
  }

  public abstract class BaseViewAction implements ViewAction {

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

  }
}
