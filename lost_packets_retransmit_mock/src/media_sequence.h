#ifndef MEDIA_SEQUENCE_H_
#define MEDIA_SEQUENCE_H_
class MediaSequence
{
public:
  explicit MediaSequence(const int low_bound, const int high_bound, const int index) {
    low_bound_ = low_bound;
    high_bound_ = high_bound >= 0 ? high_bound : 65535;
    high_bound_ = high_bound_ >= low_bound_ ? high_bound_ : low_bound_;
    if ((index >= low_bound_) && (index <= high_bound_)) {
      index_ = index;
    } else {
      index_ = low_bound_;
    }
    mid_number_ = (high_bound_ + low_bound_) / 2;
    band_width_ = high_bound_ - low_bound_ + 1;
  };

  explicit MediaSequence(const int index) {
    low_bound_ = 0;
    high_bound_ = 65535;
    mid_number_ = (high_bound_ + low_bound_) / 2;
    if ((index >= low_bound_) && (index <= high_bound_)) {
      index_ = index;
    } else {
      index_ = low_bound_;
    }
    band_width_ = high_bound_ - low_bound_ + 1;
  };

  ~MediaSequence() {};

  inline MediaSequence operator = (const int index) {
    if ((index >= low_bound_) && (index <= high_bound_)) {
      index_ = index;
    } else {
      index_ = low_bound_;
    }
    return *this;
  };
  // MediaSequence <= MediaSequence
  inline bool operator < (const MediaSequence &ms) const {
    return (CalculateDifference(index_, ms.index_) < 0);
  }
  inline bool operator > (const MediaSequence &ms) const {
    return (CalculateDifference(index_, ms.index_) > 0);
  }
  inline bool operator <= (const MediaSequence &ms) const {
    return (CalculateDifference(index_, ms.index_) <= 0);
  }
  inline bool operator >= (const MediaSequence &ms) const {
    return (CalculateDifference(index_, ms.index_) >= 0);
  }
  inline bool operator == (const MediaSequence &ms) const {
    return (CalculateDifference(index_, ms.index_) == 0);
  }
  // MediaSequence <= int
  inline bool operator < (const int &sequence) const {
    return (CalculateDifference(index_, sequence) < 0);
  }
  inline bool operator > (const int &sequence) const {
    return (CalculateDifference(index_, sequence) > 0);
  }
  inline bool operator <= (const int &sequence) const {
    return (CalculateDifference(index_, sequence) <= 0);
  }
  inline bool operator >= (const int &sequence) const {
    return (CalculateDifference(index_, sequence) >= 0);
  }
  inline bool operator == (const int &sequence) const {
    return (CalculateDifference(index_, sequence) == 0);
  }

  inline MediaSequence operator ++ (int) {
    index_++;
    index_ = index_ > high_bound_ ? low_bound_ : index_;
    return *this;
  }
  inline MediaSequence operator -- (int) {
    index_--;
    index_ = index_ < low_bound_ ? high_bound_ : index_;
    return *this;
  }

  inline int operator - (const int &differ) const {
    // Differ is need checking.
    int tmp = index_ - differ;
    return (tmp < low_bound_ ? (band_width_ + tmp ) : tmp);
  }

  inline int operator + (const int &differ) const {
    // Differ is need checking.
    int tmp = index_ + differ;
    return (tmp > high_bound_ ? (tmp - band_width_) : tmp);
  }

  inline int operator - (const MediaSequence &another) const {
    //TODO(Chaos):Is need to check the another has the same bound?
    return CalculateDifference(index_, another.index_);
  }

  inline int GetIndex() const{
    return index_;
  }

private:
  inline int CalculateDifference(const int a, const int b) const {
    int difference = a - b;
    if (difference > mid_number_) {
      difference = a - band_width_ - b;
    } else if (difference < -mid_number_) {
      difference = a + band_width_ - b;
    }
    return difference;
  }

  int index_;
  int low_bound_;
  int high_bound_;
  int mid_number_;
  int band_width_;
};

#endif // !MEDIA_SEQUENCE_H_
