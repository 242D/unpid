#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

extern bool isInRange(float value, float min, float max);
extern int randRange(int min, int max);
extern bool isEstimatedEqual(int value, int estimate);

struct OffsetData {
    int max;
};
struct RangeData {
    int min;
    int max;
};
struct TrainingData {
    int min = 1;
    int max = 10;
    OffsetData offset;
    RangeData range;
};

class UNPID {
    private:
        float offset;
        float max;
        float range;
        float value;
        float lastAverageOffset;
        float lastAccurateOffset;
        TrainingData trained;
        bool isTraining = false;
        float lastOffset = 0;
        int roundsSinceLastOffsetChange = 0;
        bool returnTrueNextValue = false;

    public:
        UNPID(int max, int range) {
            this->max = max;
            this->range = range;
            this->offset = 0;
            this->value = 0;
        }

        int getAverageOffset() {
            return this->lastAverageOffset;
        }

        int getAccurateOffset() {
            return this->lastAccurateOffset;
        }

        int getMax() {
            return this->max;
        }

        int getRange() {
            return this->range;
        }

        int getValue() {
            return this->value;
        }

        int getOffset() {
            return this->offset;
        }

        void setMax(int value) {
            this->max = value;
        }

        void setRange(int value) {
            this->range = value;
        }

        void setValue(int value) {
            this->value = value;
        }

        void pretrain(TrainingData increment, int iterations, bool linear = false) {
            this->trained = increment;
            for (int i = 0; i < iterations; i++) {
                int incrementS = 0;
                if (linear) {
                    incrementS = increment.min;
                } else {
                    incrementS = randRange(increment.min, increment.max);
                }
                this->update(this->value + incrementS, true, true);
            }
            this->reset();
        }

        bool update(int value, bool fallbackDoTrain = true, bool forceEquality = false) {
            this->value = value;
            bool conditionEstimate = forceEquality && !(isEstimatedEqual(value, this->max + this->offset) || isEstimatedEqual(value, this->max - this->offset));
            bool conditionForcedEquality = forceEquality && (value == this->max + this->offset || value == this->max - this->offset);
            if (this->max == this->offset + value || this->max == this->offset - value) {
                this->returnTrueNextValue = false;
                return true;
            }
            if (isEstimatedEqual(value, this->max + this->offset) || isEstimatedEqual(value, this->max - this->offset)) {
                this->offset = this->max - value;
                this->lastAverageOffset = this->offset;
                this->lastAccurateOffset = this->offset;
                this->range -= 1;
                if (this->range < this->trained.range.min) {
                    this->range = this->trained.range.min;
                }
                return true;
            }
            if (value > this->max + this->range) {
                this->value = this->max + this->range;
                int oldValue = this->value;
                if (fallbackDoTrain && !this->isTraining) {
                    this->isTraining = true;
                    while (this->offset > this->trained.offset.max) {
                        this->update(this->value + randRange(this->trained.min, this->trained.max), false, true);
                    }
                    this->value = oldValue;
                    this->isTraining = false;
                }
            }
            if (isInRange(value - this->offset, this->max - this->range, this->max + this->range) || isInRange(value + this->offset, this->max - this->range, this->max + this->range)) {
                this->offset = ceil((this->offset + (value - this->max)) / 2);
                this->lastAverageOffset = this->offset;
                this->lastAccurateOffset = this->max - value;
                this->range -= 1;

                if (this->range < this->trained.range.min) {
                    this->range = this->trained.range.min;
                }
                
                if (abs(this->lastAverageOffset) < abs(this->lastAccurateOffset)) {
                    this->offset = this->lastAverageOffset;
                } else {
                    this->offset = this->lastAccurateOffset;
                }

                if (this->max == this->lastAccurateOffset + value || this->max == this->lastAccurateOffset - value) {
                    this->offset = this->lastAccurateOffset;
                    this->returnTrueNextValue = true;
                    return false;
                } else if (this->max == this->lastAverageOffset + value || this->max == this->lastAverageOffset - value) {
                    this->offset = this->lastAverageOffset;
                    this->returnTrueNextValue = true;
                    return false;
                }
                return true;
            } else if (this->range < this->trained.range.max) {
                this->range += 1;
            }
            return false;
        }

        void reset() {
            this->value = 0;
        }

        void resetTraining() {
            this->offset = 0;
            this->value = 0;
            this->range = this->trained.range.min;
        }
};

extern int randomBelievableIncrement();