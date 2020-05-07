//$Id: clock.cc 836 2007-05-09 03:02:38Z jjongsma $ -*- c++ -*-

/* gtkmm example Copyright (C) 2002 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>
#include "clock.h"


Clock::Clock()
: m_radius(0.42), m_line_width(0.025)
{
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &Clock::on_timeout), 15 );
}

Clock::~Clock()
{
}

bool Clock::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  // scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
  // the center of the window
  cr->scale(width, height);
  cr->translate(0.5, 0.5);
  cr->set_line_width(m_line_width);

  cr->save();
  cr->set_source_rgba(0.337, 0.612, 0.117, 0.0);   // green
  cr->paint();
  cr->restore();
  cr->arc(0, 0, m_radius, 0, 2 * M_PI);
  cr->save();
  cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
  cr->fill_preserve();
  cr->restore();
  cr->stroke_preserve();
  cr->clip();

  cr->save();
  //cr->set_line_cap(Cairo::LINE_CAP_ROUND);
  double dotSize = 0.04;

  for(int i = 0; i < points.size(); i++){
    // draw a little dot in the middle
	double r = 0.35;
	double x = r*sin(M_PI/180*points[i]);
	double y = -r*cos(M_PI/180*points[i]);
	//points[i] += 5;

    cr->translate(x, y);
    cr->set_source_rgba(0.5, 0.5, 0.9, 1.0);
    cr->arc(0, 0, dotSize , 0, 2 * M_PI);
    cr->fill();
    cr->translate(-x, -y);
  }

  // draw a little dot in the middle
  cr->set_source_rgba(0.9, 0.5, 0.5, 1.0);
  cr->arc(0, 0, dotSize , 0, 2 * M_PI);
  cr->fill();
  cr->restore();

  return true;
}


bool Clock::on_timeout()
{
		points.clear();
		int sizeS = interfacer->anime->getSources(interfacer->anime->time).size();
		for(int i = 0; i < sizeS; i++){
			points.push_back(interfacer->anime->getSources(interfacer->anime->time).at(i)->getProperties()->position->theta);
		}
    // force our program to redraw the entire clock.
    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
    return true;
}
