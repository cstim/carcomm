package de.cstimming.konphidroid.test;

import java.util.ArrayList;
import java.util.List;

import android.location.Location;
import junit.framework.TestCase;
import de.cstimming.konphidroid.GpxCharArrayWriter;

public class GpxFileTest extends TestCase {
	private List<Location> m_list;

	protected void setUp() throws Exception {
		super.setUp();
		m_list = new ArrayList<Location>();
		Location loc = new Location("foo");
		loc.setLongitude(0);
		loc.setLatitude(0);
		m_list.add(loc);
		m_list.add(loc);
	}

	public void testWriter() {
		GpxCharArrayWriter w = new GpxCharArrayWriter(m_list);
		System.out.println(w.toString());
		System.out.println("blabla");
		assertTrue(true);
	}
	protected void tearDown() throws Exception {
		m_list = null;
		super.tearDown();
	}

}
