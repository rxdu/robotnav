/*
 * quad_flattraj.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: rdu
 */

#include <iostream>

#include "polyopt/quad_flattraj.h"

using namespace srcl_ctrl;

QuadFlatTraj::QuadFlatTraj()
{

}

QuadFlatTraj::~QuadFlatTraj()
{

}

void QuadFlatTraj::AddTrajSeg(const std::vector<std::vector<double>>& seg_coeffs, double ts, double te)
{
	if(seg_coeffs.size() != 4) {
		std::cerr << "data incomplete, failed to add trajectory segment" << std::endl;
		return;
	}

	QuadFlatOutputSeg seg;
	seg.seg_x = PolyTrajCurve(seg_coeffs[0], true, ts, te);
	seg.seg_y = PolyTrajCurve(seg_coeffs[1], true, ts, te);
	seg.seg_z = PolyTrajCurve(seg_coeffs[2], true, ts, te);
	seg.seg_yaw = PolyTrajCurve(seg_coeffs[3], true, ts, te);

	seg.t_start = ts;
	seg.t_end = te;

	traj_segs_.push_back(seg);
}

QuadFlatOutput QuadFlatTraj::GetTrajPointPos(double t)
{
	QuadFlatOutput state;

	if(t < traj_segs_.front().t_start)
	{
		state.x = traj_segs_.front().seg_x.GetCurvePointPos(t);
		state.y = traj_segs_.front().seg_y.GetCurvePointPos(t);
		state.z = traj_segs_.front().seg_z.GetCurvePointPos(t);
		state.yaw = traj_segs_.front().seg_yaw.GetCurvePointPos(t);
	}
	else if(t > traj_segs_.back().t_end)
	{
		state.x = traj_segs_.back().seg_x.GetCurvePointPos(t);
		state.y = traj_segs_.back().seg_y.GetCurvePointPos(t);
		state.z = traj_segs_.back().seg_z.GetCurvePointPos(t);
		state.yaw = traj_segs_.back().seg_yaw.GetCurvePointPos(t);
	}
	else
	{
		// TODO here assumes the trajectory is continuous
		for(auto& seg:traj_segs_)
		{
			if(t >= seg.t_start && t <= seg.t_end)
			{
				state.x = seg.seg_x.GetCurvePointPos(t);
				state.y = seg.seg_y.GetCurvePointPos(t);
				state.z = seg.seg_z.GetCurvePointPos(t);
				state.yaw = seg.seg_yaw.GetCurvePointPos(t);

				break;
			}
		}
	}

	return state;
}
