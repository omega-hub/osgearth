/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2013 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "TritonNode"
#include "TritonContext"
#include "TritonDrawable"
#include <osgEarth/CullingUtils>
#include <Triton.h>

#define LC "[TritonNode] "

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Drivers::Triton;

TritonNode::TritonNode(const Map*           map,
                       const TritonOptions& options) :
_options( options )
{
    _TRITON = new TritonContext( options );
    _TRITON->setSRS( map->getSRS() );

    osg::Geode* geode = new osg::Geode();
    geode->setCullingActive( false );
    geode->addDrawable( new TritonDrawable(_TRITON) );

    this->addChild( geode );
    
    // Triton requires an update pass.
    //ADJUST_UPDATE_TRAV_COUNT(this, +1);
}

TritonNode::~TritonNode()
{
    //nop
}

void
TritonNode::onSetSeaLevel()
{
    if ( _TRITON->ready() )
        _TRITON->getEnvironment()->SetSeaLevel( getSeaLevel() );
}

void
TritonNode::traverse(osg::NodeVisitor& nv)
{
    if ( nv.getVisitorType() == nv.CULL_VISITOR && _options.maxAltitude().isSet() )
    {
        osg::Vec3 eye = nv.getEyePoint();
        double alt = eye.z();
        if ( _TRITON->getSRS()->isGeographic() )
            alt = eye.length() - _TRITON->getSRS()->getEllipsoid()->getRadiusEquator();
        if ( alt > _options.maxAltitude().get() )
            return;
    }

#if 0
    if ( _TRITON->ready() )
    {
        if ( nv.getVisitorType() == nv.UPDATE_VISITOR )
        {
            _TRITON->update( nv.getFrameStamp()->getSimulationTime() );
        }
    }
#endif
    osgEarth::Util::OceanNode::traverse( nv );
}