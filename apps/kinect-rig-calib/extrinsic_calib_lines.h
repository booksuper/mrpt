/*
 *  Copyright (c) 2013, Universidad de Málaga  - Grupo MAPIR
 *                      INRIA Sophia Antipolis - LAGADIC Team
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the holder(s) nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Author: Eduardo Fernandez-Moral
 */

#pragma once

#include "extrinsic_calib.h"
#include "line_corresp.h"
#include <mrpt/system/filesystem.h>
#include <mrpt/math/lightweight_geom_data.h>
#include <mrpt/math/CMatrixTemplateNumeric.h>  // For mrpt::math::CMatrixDouble
#include <mrpt/vision/chessboard_stereo_camera_calib.h>
#include <opencv2/core/core.hpp>
#include <Eigen/Dense>

/*! This class contains the functionality to calibrate the extrinsic parameters of a pair of non-overlapping depth cameras.
 *  This extrinsic calibration is obtained by matching lines that are observed by both sensors at the same time instants.
 *
 *  \ingroup calib_group
 */
template<typename T>
class ExtrinsicCalibLines : public virtual ExtrinsicCalib<T>
{
private:

    using ExtrinsicCalib<T>::num_sensors;
    using ExtrinsicCalib<T>::Rt_estimated;

    size_t min_pixels_line;
    std::vector< std::vector<cv::Vec4i> > v_segments2D;
    cv::Vec4i line_match1;
    cv::Vec4i line_match2;
    std::vector< std::vector<Eigen::Vector3f> > v_segment_n; // The normal vector to the plane containing the 2D line segment and the optical center
    std::vector< std::vector<mrpt::math::TLine3D> > v_lines3D;
    std::vector< std::vector<Eigen::Matrix<T,6,1> > > v_segments3D;
    std::vector< std::vector<bool> > v_line_has3D;

    /*! Indices of the candidate correspondences */
    std::array<size_t,2> line_candidate;
//    std::map<unsigned, unsigned> line_corresp;

public:

//    /*! The current extrinsic calibration parameters */
//    mrpt::math::TLine3D makeTLine3D(const Eigen::Vector3f & p1, const Eigen::Vector3f & p2)
//    {
//        mrpt::math::TLine3D line;
//    }

    /*! The current extrinsic calibration parameters */
//    ExtrinsicCalib<T> * calib;

    /*! The plane correspondences between the different sensors */
    LineCorresp<T> lines;

    /*! The coordinates of the optical center of the rgb cameras */
    std::vector<float> cx, cy;

    /*! Constructor */
    ExtrinsicCalibLines(const ExtrinsicCalib<T> * cal, std::vector<mrpt::utils::TStereoCamera> intrinsics) : calib(cal), min_pixels_line(120)
    {
//            std::map<unsigned, std::map<unsigned, mrpt::math::CMatrixDouble> > mm_corresp_;

//            /*! Conditioning numbers used to indicate if there is enough reliable information to calculate the extrinsic calibration */
//            std::vector<float> conditioning;

//            /*! Rotation covariance matrices from adjacent sensors */
//            std::vector<Eigen::Matrix3f, Eigen::aligned_allocator<Eigen::Matrix3f> > covariances;
    }

    /*! Extract 3D lines from 2D segments in a RGB-D image.*/
    static void getSegments3D(const TCamera & cam, const pcl::PointCloud<PointT>::Ptr & cloud, const mrpt::pbmap::PbMap & pbmap, const vector<cv::Vec4i> & segments2D,
                              const std::vector<Eigen::Vector3f> & segments_n, vector<Matrix<T,6,1> > & segments3D, vector<bool> & line_has3D);

    /*! Extract line correspondences between the different sensors.*/
    void getCorrespondences(const std::vector<cv::Mat> & rgb, const std::vector<pcl::PointCloud<PointT>::Ptr> & cloud);

    /*! Calculate the angular error of the plane correspondences.*/
    float calcRotationError(Eigen::Matrix3f &Rot_);

    /*! \overload Calculate the angular error of the plane correspondences.*/
    inline float calcRotationError(Eigen::Matrix4f &Rt_)
    {
        Eigen::Matrix3f R = Rt_.block(0,0,3,3);
        return calcRotationError(R);
    }

    /*! \overload Calculate the angular error of the plane correspondences.*/
    inline float calcRotationError()
    {
        Eigen::Matrix3f R = Rt_estimated.block(0,0,3,3);
        return calcRotationError(R);
    }

    //    float calcCorrespTransError(Eigen::Matrix3f &Rot_)

    //    Eigen::Matrix3f calcFisherInfMat(const int weightedLS = 0)

    /*! Calibrate the relative rotation of the pair. */
    Eigen::Matrix3f CalibrateRotation(int weightedLS = 0);

    /*! \overload Calibrate the relative rotation of the pair (double precision). */
    Eigen::Matrix3f CalibrateRotationD(int weightedLS = 0);

    /*! Calibrate the relative rotation of the pair iteratively on a manifold formulation. */
    Eigen::Matrix3f CalibrateRotationManifold(int weightedLS = 0);

    /*! Calibrate the relative translation of the pair. */
    Eigen::Vector3f CalibrateTranslation(int weightedLS = 0);

    /*! Calibrate the relative rigid transformation (Rt) of the pair. */
    void CalibratePair();

};
