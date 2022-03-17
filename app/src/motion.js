class motion {
    constructor() {
      this.cacheLength = 300;
      this.inputCache = [];
      this.outputCache = []
  
      this.samplingRate = 30
      
      this.filteredData = 0
      this.pidData = 0
      this.direction = 0
      
      this.motionFormer = 0
      this.motionAvgDelay = 0
      this.motionAvgAvg = 0
  
      // filter using python
      // b = signal.firwin(5, 0.1)
      // z = signal.lfilter_zi(b, 1) * motionFormer
      
      //current action stage presist length
      this.actionStepLength = 0
      //stage change vulve values
      this.vulve = 5
      //angle direction 1 for up -1 for down 
      this.previousDirection = 0
      this.stepDirection = 0
      this.descendingMin = 0
      this.ascendingMax = 0
      this.filterRead = 0 
      this.directionChange = 0
    }
    newImpulse(nextRead, threshold){
        this.vulve = threshold;
        if(this.motionFormer == 0) this.motionFormer = nextRead;
        this.filteredData = this.filter(nextRead, this.motionFormer, 0.8);
        this.motionFormer = nextRead;
        //self.filtered_data, self.z = signal.lfilter(self.b, 1, [next_read], zi=self.z)
        var p = this.filteredData;
        var d = this.filteredData - this.motionFormer;
        
        // direction judgement
        var motionDirection = Math.sign(d);
        if(this.previousDirection!= motionDirection) this.directionChange = true;
        this.previousDirection = motionDirection;     

        // if tracking lost use current direction and next
        if(this.stepDirection == 0) this.stepDirection = motionDirection;
  
        //maintain max and min value in a step 
        if(this.stepDirection == 1){
            this.ascendingMax = Math.max(this.ascendingMax, p);
            this.pidData = this.ascendingMax;
        }
        if(this.stepDirection == -1){
            this.descendingMin = Math.min(this.descendingMin, p);
            this.pidData = this.descendingMin;            
        }

        // when direction change exame vulve value, to check if direction changed 
        if(this.directionChange && motionDirection != this.stepDirection){
            if(motionDirection == 1 && p - this.descendingMin > this.vulve){
                this.actionStepLength = 0;
                this.stepDirection = motionDirection;
                this.ascendingMax = p;
                this.descendingMin = p;
                // real direction changed print
            }
                
            if(motionDirection == -1 && this.ascendingMax - p > this.vulve){
                this.actionStepLength = 0;
                this.stepDirection = motionDirection;
                this.ascendingMax = p;
                this.descendingMin = p; 
                // real direction changed print
            }
        }
        this.direction = this.stepDirection;
        this.actionStepLength = this.actionStepLength + 1;
        
        if(this.inputCache.length == 280) this.inputCache.splice(0,1);
        else this.inputCache.push(nextRead);
        
        if(this.outputCache.length == 280) this.outputCache.splice(0,1);
        else this.outputCache.push(this.direction);
        // todo: debug mode   
        this.cacheLength = this.cacheLength - 1;
        var res = [this.direction, this.pidData];
        return res;

    }
    filter(read, pre, damping){
        var res = pre * damping + read * (1 - damping);
        return res;
    }
}

module.exports = motion;
