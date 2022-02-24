from numpy import array, zeros
from matplotlib import pyplot as plt
plt.ion()

# Define numbers of tests performed & number of samples per test
nTest = 99
perTest = 204
mWeights = array([.09,.01,.9])

# Define a class for reading & storing series of network test data
class series:

    """A series of test results for a Myc-Drop network."""
    def __init__(self,testDir,label,inputs,topology,alpha,nTest):

        # Assign the label & other identifiers provided for this series of tests
        self.label = label
        self.inputs = inputs
        self.topology = topology
        self.alpha = alpha

        # Initialize containers for results
        self.results = zeros((4,nTest))

        # Extract results from test files
        self.readResults(testDir,nTest)

    # Extracts results from test files
    def readResults(self,testDir,nTest):
        for i in range(nTest):
            resLines = open(testDir+"_%02d.txt"%(i+1)).readlines()
            self.results[0,i] = int(resLines[1].strip().split()[1])
            self.results[1,i] = int(resLines[2].strip().split()[2])
            self.results[2,i] = int(resLines[3].strip().split()[2])
        self.results /= perTest
        self.results[3,:] = mWeights@array([self.results[0,:],1-self.results[1,:],1-self.results[2,:]])

# Define the various test conditions (inputs, architecture, learning rate)
inConds = ["2Key","6Key","All"]
l1Conds = ["1","2","3"]
l2Conds = ["","-1","-2","-3"]
aConds = ["aLo","aHi"]
learnRuns = ["2","3-2","3-1"]
inputs = [[2,21],[2,4,12,13,19,21],list(range(22))]

# Looping over test conditions, add test runs to a list
runList = []
for i in range(len(inConds)):
    iCond = inConds[i]

    # Add runs using typical learning rates
    for l1Cond in l1Conds:
        for l2Cond in l2Conds:
            topology = tuple([int(s) for s in (l1Cond+l2Cond).split("-")])
            netName = iCond+"_"+l1Cond+l2Cond
            testDir = "results/"+iCond+"/"+netName+"/summary_"+netName
            runList.append(series(testDir,netName,inputs[i],topology,0.25,nTest))

    # Add runs using higher & lower learning rates
    topology = tuple([int(s) for s in learnRuns[i].split("-")])
    for a in range(2):
        netName = iCond+"_"+learnRuns[i]+"_"+aConds[a]
        testDir = "results/"+iCond+"/"+netName+"/summary_"+netName
        runList.append(series(testDir,netName,inputs[i],topology,0.1+.4*a,nTest))

# Count runs & define labels for each test/result
nRun = len(runList)
netLabs = array([li.label for li in runList])
resLabs = array(["Overall Success Rate","False Positive Rate",'False Negative Rate','Combined Score'])

# Report the most performant networks by each metric
outFile = open("top-results.txt","w")
outFile.write("TOP RESULTS\n")
for r in range(4):
    rates = array([li.results[r].mean() for li in runList])
    errs = array([1.96*li.results[r].std() for li in runList])
    winners = array(range(nRun))[rates==max(rates)] if (r in [0,3]) else array(range(nRun))[rates==min(rates)]
    outFile.write("\n"+resLabs[r]+":\n")
    for w in winners: outFile.write("\n\t"+netLabs[w].ljust(15)+"\t(%.3f +/- %.3f)\n"%(rates[w],errs[w]))
outFile.close()

# Plot results by architecture
for n in range(3):
    nIn = len(inputs[n])
    fig, axs = plt.subplots(3,3)
    fig.suptitle(r"Results by Architecture ($N_{in}$ = %d)"%nIn,fontsize=20)
    ii=0
    for i in [0,2,3]:
        for j in range(3):
            results = []
            nl2 = []
            for li in runList:
                if (li.topology[0]==j+1 and li.alpha==.25 and len(li.inputs)==nIn):
                    results.append(li.results[i])
                    if len(li.topology)>1: nl2.append(li.topology[1])
                    else: nl2.append(0)
            #if i==0: axs[0,j].set_title(r"(%d, $N_2$)"%(j+1),fontsize=14,pad=10)
            if i==0: axs[0,j].set_title(r"$N_1$ = %d"%(j+1),fontsize=14,pad=10)
            axs[ii,j].boxplot(results,showbox=True,showmeans=True,meanline=True,\
                meanprops=dict(color='r',ls='-',lw=1.5),medianprops=dict(color='b',lw=1.5))
            axs[ii,j].set_xticklabels(nl2)
            axs[ii,j].set_yticks([0,0.5,1.0])
            axs[ii,j].axis([.5,len(nl2)+.5,-.01,1.01])
            axs[ii,j].tick_params(direction='in',labelsize=12)
            axs[ii,j].tick_params(axis='x',length=0)
        axs[ii,0].set_ylabel(resLabs[i],fontsize=16,labelpad=15)
        ii+=1
    axs[2,1].set_xlabel(r"$N_2$",fontsize=18,labelpad=10)
    fig.subplots_adjust(left=.08,bottom=.08,right=.98,top=.88,hspace=.25)
    fig.show()

# Plot results by learning rate for top performers by inputs
fig, axs = plt.subplots(3,3)
fig.suptitle("Results by Learning Rate",fontsize=20)
ii=0
for i in [0,2,3]:
    for j in range(3):
        nIn = len(inputs[j])
        topology = tuple([int(s) for s in learnRuns[j].split("-")])
        topoStr = str(topology) if len(topology)>1 else str(topology+tuple([0]))
        results = array([li.results[i] for li in runList if (li.topology==topology and len(li.inputs)==nIn)])
        alphas = array([li.alpha for li in runList if (li.topology==topology and len(li.inputs)==nIn)])
        aOrder = alphas.argsort(axis=0)
        if i==0: axs[0,j].set_title(r"$N_{in}$=%d,  ($N_1$, $N_2$)="%nIn+topoStr,fontsize=14,pad=10)
        axs[ii,j].boxplot(results[aOrder].T,showbox=True,showmeans=True,meanline=True,\
            meanprops=dict(color='r',ls='-',lw=1.5),medianprops=dict(color='b',lw=1.5))
        axs[ii,j].set_xticks(range(1,len(alphas)+1))
        axs[ii,j].set_xticklabels([str(a) for a in alphas[aOrder]])
        axs[ii,j].set_yticks([0,0.5,1.0])
        axs[ii,j].axis([.5,len(results)+.5,-.01,1.01])
        axs[ii,j].tick_params(direction='in',labelsize=12)
        axs[ii,j].tick_params(axis='x',length=0)
    axs[ii,0].set_ylabel(resLabs[i],fontsize=16,labelpad=15)
    ii+=1
axs[2,1].set_xlabel(r"$\alpha$",fontsize=18,labelpad=10)
fig.subplots_adjust(left=.08,bottom=.08,right=.98,top=.88,hspace=.25)
fig.show()