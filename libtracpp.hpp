#define MAX_DIST_ORTHO           5         
#define MIN_DIST_ORTHO           10000     
#define MAX_COV_DIST             15        
#define MIN_COV_DIST             50        
#define MIN_RECORD_PER_TRIP      5
#define DEG_TO_RAD               1.745329e-2
#define GEODESIC_DEG_TO_M        111070.4   // conversion [deg] -> [meter] on equatorial circle */

template<class T>
T center_of_mass(const std::vector<T> &data)
{
  // maybe add some safety on data.size() throwing stuff?
  T com;
  com.lat = 0.0;
  com.lon = 0.0;
  for (const auto & record : data)
  {
    com.lat += record.lat;
    com.lon += record.lon;
  }
  com.lat /= (double)data.size();
  com.lon /= (double)data.size();

  return com;
}

template<class T>
double cov_dist_ij(const std::vector<T> &data, int i, int j) // distance from index=i to index=j
{
  double dist = 0, dx, dy;
  for (int k = i; k < j; ++k) 
  {
    dx = GEODESIC_DEG_TO_M*cos(data[k].lat*DEG_TO_RAD)*(data[k + 1].lon - data[k].lon);
    dy = GEODESIC_DEG_TO_M*(data[k + 1].lat - data[k].lat);
    dist += sqrt(dx*dx + dy*dy);
  }
  return dist;
}

class rdp_algo {
public:
  double max_cov_dist;            // d max tra punti successivi che innesca restore_points [meters]     
  double min_cov_dist;            // d minima tra punti successivi per cui l'algoritmo non viene lanciato [meters] 
  double max_ortho_dist;          // distanza massima ortogonale per selezionare un punto [meters]
  double min_ortho_dist;          // distanza minima ortogonale (deprecated) [meters]             
  size_t min_record_per_trip;

  rdp_algo() {
    max_cov_dist = MAX_COV_DIST;
    min_cov_dist = MIN_COV_DIST;
    max_ortho_dist = MAX_DIST_ORTHO;
    min_ortho_dist = MIN_DIST_ORTHO;
    min_record_per_trip = MIN_RECORD_PER_TRIP;
  }

  rdp_algo(double _max_cov_dist, double _min_cov_dist, double _max_ortho_dist, double _min_ortho_dist, size_t _min_record_per_trip) {
    max_cov_dist = _max_cov_dist;
    min_cov_dist = _min_cov_dist;
    max_ortho_dist = _max_ortho_dist;
    min_ortho_dist = _min_ortho_dist;
    min_record_per_trip = _min_record_per_trip;
  }

  template<class T>
  void rdp_engine_recursive(bool * status, const std::vector<T> &data, int index1, int index2) {

    if (cov_dist_ij(data, index1, index2) < min_cov_dist) 
    {
      for (int i = index1 + 1; i < index2; ++i) status[i] = false;
      return;
    }

    double dx = GEODESIC_DEG_TO_M*cos(data[index2].lat*DEG_TO_RAD)*(data[index2].lon - data[index1].lon);
    double dy = GEODESIC_DEG_TO_M*(data[index2].lat - data[index1].lat);
    double ds = sqrt(dx*dx + dy*dy);

    if (!(ds > 0)) throw(int(123));

    dx /= ds;
    dy /= ds;
    double dmax = -1;
    double coslat1 = cos(data[index1].lat*DEG_TO_RAD);
    int iw = -1;
    double dxw, dyw, dsw;

    for (int i = index1 + 1; i < index2; ++i) 
    {
      dxw = GEODESIC_DEG_TO_M*coslat1*(data[i].lon - data[index1].lon);
      dyw = GEODESIC_DEG_TO_M*(data[i].lat - data[index1].lat);
      dsw = fabs(dxw*dy - dyw*dx);
      if (dsw > dmax) 
      {
        dmax = dsw;
        iw = i;
      }
    }
    if ((dmax > max_ortho_dist || (cov_dist_ij(data, index1, index2) > max_cov_dist && dmax > min_ortho_dist))) 
    {
      status[iw] = true;
      rdp_engine_recursive(status, data, index1, iw);
      rdp_engine_recursive(status, data, iw, index2);
    }
    else 
    {
      for (int i = index1 + 1; i < index2; ++i) 
      {
        status[i] = false;
      }
    }
    return;
  }

  template<class T>
  void restore_points_smart(bool * status, const std::vector<T> &data) 
  {
    int index1 = 0, index2 = 0;
    for (size_t i = 1; i <= data.size() - 1; ++i) 
    {
      if (status[i]) 
      {
        index2 = int(i);
        double dist = cov_dist_ij(data, index1, index2);
        if (dist > max_cov_dist) {
          double delta = (dist) / int((dist / (max_cov_dist)+1)) + 1.;
          double ds = 0;
          double counter = 1.;
          for (int k = index1 + 1; k < index2; ++k) 
          {
            ds += cov_dist_ij(data, k - 1, k);
            if (ds > delta*counter) 
            {
              status[k] = true;
              counter++;
            }
          }
        }
        index1 = index2;
      }
    }
    return;
  }

  template<class T>
  std::vector<T> reduce(const std::vector<T> &data) 
  {
    std::vector<T> reduced;
    if (data.size() > min_record_per_trip) 
    {
      bool * status = new bool[data.size()];
      memset(status, 1, data.size() * sizeof(bool));

      try 
      {
        rdp_engine_recursive(status, data, 0, int(data.size() - 1) );
      }
      catch (...) 
      {
        reduced = data;
        return reduced;
      }

      restore_points_smart(status, data);

      for (size_t i = 0; i < data.size(); ++i) 
      {
        if (status[i]) reduced.push_back(data[i]);
      }
    }
    else 
    {
      reduced = data;
    }

    return reduced;
  }

};
