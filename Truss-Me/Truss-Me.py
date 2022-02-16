from numpy import array, zeros, outer, block
from numpy.linalg import inv
from matplotlib import pyplot as plt
plt.ion()

# Define the 2-node bar element
class bar2 :

	"""A two-node bar element."""
	def __init__(self,Y,rho,A,position):

		# Assign bar2 parameters
		self.Y = Y
		self.rho = rho
		self.A = A
		self.rs = position

		# Initialize internal forces & stresses
		self.FInt = 0
		self.sigInt = 0

		# Call bar2 analysis functions
		self.getL()
		self.getWeight()
		self.getK()

	# Computes & assigns the length of the bar
	def getL(self):
		self.span = self.rs[1]-self.rs[0]
		self.L = (self.span@self.span)**.5

	# Computes & assigns the bar's weight
	def getWeight(self):
		self.W = self.L*self.A*self.rho

	# Computes & assigns the bar's Y matrix
	def getK(self):
		C = self.Y*self.A/self.L**3
		sub = outer(self.span,self.span)
		self.K = C*block([[sub,-sub],[-sub,sub]])

	# Applies displacements, computing internal forces & stresses
	def applyDisp(self,dr):
		dL = array([dr[i+3]-dr[i] for i in range(3)])
		C = self.Y*self.A/self.L**2
		self.FInt = C*(self.span@dL)
		self.sigInt = self.FInt/self.A

	# Resets internal forces & stresses
	def reset(self):
		self.FInt = 0
		self.sigInt = 0

# Define a truss structure composed of 2-node bar elements
class bar2Truss:

    """A truss structure made up of bar2 elements."""

    def __init__(self,stiffnesses,densities,areas,rNodes,eNodes):

        # Assign node positions
        self.rNodes = rNodes
        self.nNodes = len(rNodes)

        # Assign element nodes
        self.eNodes = eNodes
        self.nEl = len(eNodes)

        # Initialize node displacements & forces
        self.nUs = zeros(3*self.nNodes)
        self.nFs = zeros(3*self.nNodes)

        # Initialize list of bar2 elements
        self.els = []
        for e in range(self.nEl):
            ePos = array([self.rNodes[n] for n in self.eNodes[e]])
            self.els.append(bar2(stiffnesses[e],densities[e],areas[e],ePos))

        # Call bar2 truss analysis functions
        self.getW()
        self.getK()

    # Computes & assigns the truss' weight
    def getW(self):
        self.W = 0
        for element in self.els:
            self.W += element.W

    # Computes & assigns the truss' stiffness matrix
    def getK(self):
        self.K = zeros((3*self.nNodes,3*self.nNodes))
        for e in range(0,self.nEl):
            Ke = self.els[e].K
            for i in range(2):
                ii = self.eNodes[e,i]
                for j in range(2):
                    jj = self.eNodes[e,j]
                    self.K[3*ii:3*(ii+1),3*jj:3*(jj+1)]+=Ke[3*i:3*(i+1),3*j:3*(j+1)]

    # 
    def applyForces(self,DOF,forces):

        # Compute the modified stiffness matrix
        modK = self.K.copy()
        modK[:,DOF==0] = 0
        modK[DOF==0,:] = 0
        modK[DOF==0,DOF==0] = 1

        # Compute the modified applied force vector
        modF = forces.copy()
        modF[DOF!=0] -= self.K[DOF!=0][:,DOF==0]@forces[DOF==0]

        # Compute nodal displacements & reaction forces
        self.nUs = inv(modK)@modF
        self.nFs = self.K@self.nUs

        # Apply nodal displacements to elements
        for e in range(self.nEl):
            inds = array([[3*i,3*i+1,3*i+2] for i in self.eNodes[e]]).flatten()
            self.els[e].applyDisp(self.nUs[inds])

    # Re-initializes the truss with no nodal displacements
    def reset(self):
        self.nUs = zeros(3*self.nNodes)
        self.nFs = zeros(3*self.nNodes)
        for ele in self.els: ele.reset()

# Colors & color mapping functions used for plotting
gold = array([.95,.66,0,1.0])
fgray = array([.9,.9,.9,0.5])
def cStress(sig,sigLim):
	R = 1-min(abs(sig/sigLim),1)*(sig<0)
	G = 1-min(abs(sig/sigLim),1)
	B = 1-min(abs(sig/sigLim),1)*(sig>0)
	return array([R,G,B])

# Plots a graphical representation of a truss
def plotStruct(truss,maxDim=zeros(3),cPane=(.8,.8,.8),cEdges=(0,0,0),nLabs=True):
	if all(maxDim==0): maxDim = 1.1*truss.rNodes.max(axis=0)
	fig = plt.figure()
	ax3d = fig.add_subplot(111, projection='3d')
	ax3d.grid(False)
	if nLabs:
		ax3d.scatter(truss.rNodes[:,0],truss.rNodes[:,1],truss.rNodes[:,2],\
			color=gold,alpha=1,s=75)
		for i in range(truss.nNodes):
			ax3d.text(truss.rNodes[i,0],truss.rNodes[i,1],truss.rNodes[i,2],str(i+1),\
				color="k",ha="center",va="center",fontsize=8)
	for ele in truss.els:
		ax3d.plot(ele.rs[:,0],ele.rs[:,1],ele.rs[:,2],'k-',lw=2.5)
	ax3d.set_xlim(-maxDim[0],maxDim[0])
	ax3d.set_ylim(-maxDim[1],maxDim[1])
	ax3d.set_zlim(0,maxDim[2])
	for ax in [ax3d.xaxis,ax3d.yaxis,ax3d.zaxis]:
		ax.set_pane_color(cPane)
		ax.pane.set_edgecolor(cEdges)
	fig.subplots_adjust(left=0.0,right=1.0,top=1.0,bottom=0.0)
	fig.show()

# Plots a graphical representation of nodal displacements in a truss
def plotDisp(truss,mag=10,maxDim=zeros(3),cPane=(.8,.8,.8,1.0),cEdges=(0,0,0,1.0)):
		if all(maxDim==0): maxDim = 1.1*truss.rNodes.max(axis=0)
		mDisp = mag*truss.nUs.reshape(truss.nNodes,3)
		drNodes = truss.rNodes+mDisp
		fig = plt.figure()
		ax3d = fig.add_subplot(111, projection='3d')
		ax3d.grid(False)
		ax3d.scatter(truss.rNodes[:,0],truss.rNodes[:,1],truss.rNodes[:,2],color=fgray,alpha=0.5,s=30)
		ax3d.scatter(drNodes[:,0],drNodes[:,1],drNodes[:,2],color=gold,alpha=1,s=30)
		for e in range(truss.nEl):
			ePos = truss.els[e].rs.reshape(len(truss.eNodes[e]),3)
			dePos = ePos+mDisp[truss.eNodes[e]]	
			ax3d.plot(ePos[:,0],ePos[:,1],ePos[:,2],'-',color=fgray,lw=2.5)
			ax3d.plot(dePos[:,0],dePos[:,1],dePos[:,2],'-',color='k',lw=2.5)
		ax3d.set_xlim(-maxDim[0],maxDim[0])
		ax3d.set_ylim(-maxDim[1],maxDim[1])
		ax3d.set_zlim(0,maxDim[2])
		for ax in [ax3d.xaxis,ax3d.yaxis,ax3d.zaxis]:
			ax.set_pane_color(cPane)
			ax.pane.set_edgecolor(cEdges)
		fig.subplots_adjust(left=0.0,right=1.0,top=1.0,bottom=0.0)
		fig.show()

# Plots a graphical representation of element stresses in a truss
def plotStress(truss,sigLim=1e3,maxDim=zeros(3),cPane=(.1,.1,.1,1.0),cEdges=(.8,.8,.8,1.0)):
	if all(maxDim==0): maxDim = 1.1*truss.rNodes.max(axis=0)
	fig = plt.figure()
	ax3d = fig.add_subplot(111, projection='3d')
	ax3d.grid(False)
	for ele in truss.els:
		ax3d.plot(ele.rs[:,0],ele.rs[:,1],ele.rs[:,2],'-',color=cStress(ele.sigInt,sigLim),lw=2.5)
	ax3d.set_xlim(-maxDim[0],maxDim[0])
	ax3d.set_ylim(-maxDim[1],maxDim[1])
	ax3d.set_zlim(0,maxDim[2])
	for ax in [ax3d.xaxis,ax3d.yaxis,ax3d.zaxis]:
		ax.set_pane_color(cPane)
		ax.pane.set_edgecolor(cEdges)
	fig.subplots_adjust(left=0.0,right=1.0,top=1.0,bottom=0.0)
	fig.show()

# Define node positions
rNodes = array([[-37.5,0,200],[37.5,0,200],[-37.5,37.5,100],[37.5,37.5,100],[37.5,-37.5,100],\
	[-37.5,-37.5,100],[-100,100,0],[100,100,0],[100,-100,0],[-100,-100,0]])
nNodes = len(rNodes)

# Define element nodes (2 per bar2 element)
eNodes = array([[0,1],[0,3],[1,2],[0,4],[1,5],[1,3],[1,4],[0,2],[0,5],[2,5],[3,4],[2,3],[4,5],\
	[2,9],[5,6],[3,8],[4,7],[3,6],[2,7],[4,9],[5,8],[5,9],[2,6],[4,8],[3,7]])
nEl = len(eNodes)

# Define element areas, stiffnesses, & densities
elAs = array([0.033,2.015,2.015,2.015,2.015,2.823,2.823,2.823,2.823,0.010,0.010,0.014,\
	0.014,0.980,0.980,0.980,0.980,1.760,1.760,1.760,1.760,2.440,2.440,2.440,2.440])
elYs = zeros(nEl)+1e7
elDens = zeros(nEl)+0.1

# Define nodal degrees of freedom (1:free; 0:fixed)
fixNodes = array([[3*i,3*i+1,3*i+2] for i in [6,7,8,9]]).flatten()
DOF = array([1 for i in range(3*nNodes)])
DOF[fixNodes] = 0

# Define applied nodal forces as a flat vector
FNodes = [0,1,2,5]
FVals = array([[1e3,1e4,-5e3],[0,1e4,-5e3],[500,0,0],[500,0,0]])
nFs = zeros((nNodes,3))
nFs[FNodes] = FVals
nFs = nFs.flatten()

# Construct a truss made up of bar2 elements
truss = bar2Truss(elYs,elDens,elAs,rNodes,eNodes)
truss.applyForces(DOF,nFs)

# Plot the truss structure & the displacements/stresses due to applied forces
plotStruct(truss,maxDim=array([250,250,250]))
plotDisp(truss,mag=100,maxDim=array([250,250,250]))
plotStress(truss,sigLim=5e3,maxDim=array([250,250,250]))