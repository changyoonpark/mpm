
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html>

<head>

<title>CS184 Milestone</title>
</head>

<body>
<h2>A Study on Realistic Physical Modeling of Snow With the Material Point Method</h2>
<h4>CHANG YOON PARK and KELLY SHEN, University of California, Berkeley</h4>

<p>In modern computer based animation, it is important to create realistic physical models of various continua, such as solids and fluids. For example, an elasto-plastic work function derived from the Prager-Drucker type plasticity crtiteria coupled with the material point method with appropriate parameters are capable of generating realistic looking snow and sand animations (Stomakhin et al.). The material point method was originally used in the engineering literature as a mesh-free method for solving continuum equations in a weak-formed fashion, similar to that of the Finite Element Method (FEM) (Sulskly et al.). Its background grid provides a subset of basis functions to evaluate various physical quantities <i>(a continuum scratch-pad)</i>, and handles collision and up to a certain amount, fracture, automatically. This makes MPM an extremely appealing approach for creating phyisical models that undergo dramatical changes in terms of geometry. It should be also noted that in MPM, we can directly work with the deformation gradient ($\mathbf{F}$).</p>
<p>
In this report, We study further on how the initial placement of the material points effect the physics of the snow. The initial configuration of the particles is a critical aspect for practical animations, and is not discussed at all in the original paper by Stomakhin. This is due to the lacking of a explicit fracture detection scheme of the implementation.</p>

<sub>Additional Key Words and Phrases: Physical Modeling, Material Point Method
<br><br><b>ACM Reference format:</b>
<br>Chang Yoon Park and Kelly Shen. 2017. A Study on Realistic Physical Modeling of Snow With the Material Point Method. 1, 1, Article 1 (April 2017), 4 pages.
<br><a href="https://doi.org/10.1145/nnnnnnn.nnnnnnn">https://doi.org/10.1145/nnnnnnn.nnnnnnn</a></sub>
<p></p>

<hr>

<h4>1&emsp;INTRODUCTION</h4>
The Material Point Method, originated from the engineering literature,
is a mesh-free method with a cartesian grid layered on top of the simulation domain.
MPM provides a robust setup for simulating various physical models, including fluids and 
solids.<br><br>
In this article, we extend on the work by Stomakhin et al., and demonstrate a physical problem involving the Newtonian physics of snow. 
<br><br>
The main work of this article can be summarized as follows:</p>
<ul>
  <li>Implementation of the Elasto-Plastic work function for the Cauchy-Stress.</li>
  <li>Semi-Implicit time stepping scheme with the Hessian of the work function, with respect to the deformation gradients of each material point.</li>
  <li>Study on how initial particle placements effect the physics.</li>
</ul>

In the original work by Stomakhin, no comments were made regarding the effects of the initial placement of the material particles, and it turns out the response of the snow is greatly dependent on the initial configurations. We will aim to provide a detailed study regarding this aspect of snow simulations, for engineers who are seeking practical snow model implementations.

<br><br>
<h4>2&emsp;SOLVING CONTINUUMS WITH MPM</h4>
<h5>2.1&emsp;Continuum Mechanics</h5>
The MPM framework provides a <i>strain-driven</i> framework which we can work with. In continuum mechanics, the deformation gradient $\mathbf{F}$ is defined as 

    $$ \mathbf{F} = \frac{\partial \mathbf{u}}{\partial \mathbf{X}} $$


where $\mathbf{u} = \overset{-}{\mathbf{u}}(\mathbf{X})$ prescribes the displacement of the material point that is located at $\mathbf{X}$ in the original configuration. 
<br>The gradient of the displacement with respect to the spatial coordinates can be obtain by exploiting the solution space defined in the background grid. The interpolation functions supporting the solutions is analogous to the interpolation functions used in the finite element formulations. 
From the deformation gradient, we can obtain all the required information regarding the deformation of the continuum body. In 3D coordinates, numerous decompositions of the deformation gradient exists. In particular, we will be using the decomposition

$$\mathbf{F}=\mathbf{RU},$$

also known as the <i>Polar Decomposition</i>. The polar decomposition states that every deformation of a infinitesimal material point can be splitted into two transformations, one involving the rotational deformation ($\mathbf{R}$) and one involving the "stretch" associated with that rotation ($\mathbf{U}$). One other decomposition we are interested in is the <i>Singular Value Decomposition</i>, also frequently abbreviated as SVD: 
$$\mathbf{F}=\mathbf{{V_1} \Sigma {V_2}^T}$$
. This decomposition returns two rotational components $\mathbf{V_1}$ \text{ and } $\mathbf{V_2}$, with a diagonal singular value tensor $\mathbf{\Sigma}$. Avoiding the details, the singular values of the deformation gradient in the context of continuum mechanics indicate the principal stretches and the pricipal compression rates of a certain material point. Employing multiplicative plastic deformation models assuming $\mathbf{F=F_E F_P}$, one can apply SVD on the elastic part of the deformation gradient $\mathbf{F_E}$ to obtain the corresponding stretches and compression rates, and if those quantities exceed the stretch/compression limit, simply dump the excessive strains onto the plastic part of the deformation gradient $\mathbf{F_P}$. One other useful physical quantity is the determinant of $\mathbf{F}$, often called the <i>Jacobian</i>:
$$J = det(\mathbf{F})$$
Physically, the Jacobian is associated with the volumetric strain of the material point.

<br><br>
<h5>2.2&emsp;Conservation Laws</h5>

In continuum mechanics, we eventually desire to solve the following PDE: 
$$ \rho \frac{D\mathbf{v}}{Dt} = -\mathbf{\nabla} \cdot \mathbf{\sigma} + \rho \mathbf{b} $$
Where $ \frac{D\mathbf{v}}{Dt} $ is referred as the "Material Derivative" which includes the convection term, and $\mathbf{\sigma}$ is the Cauchy-Stress defined in the current configuration, and $\mathbf{b}$ is the body force acting on the material point, such as gravity. Note here that the Cauchy Stress is in general, a nonlinear function with respect to the deformation and various material parameters:
$$\mathbf{\sigma} = \mathbf{\overset{-}{\sigma}}(\mathbf{F}, E, \nu, \textit{etc}...)$$

<h5>2.3&emsp;Constitutive Equations</h5>
There exists many ways to prescribe the behavior of a material under certain deformations. One way is by assuming a material "Work function" $\Psi(\mathbf{F})$. We assume the Cauchy-Stress to follow the below rule: 
$$ \mathbf{\sigma} = J^{-1}\frac{\partial \Psi(\mathbf{F})}{\partial \mathbf{F}}$$

In Stomakhin et al, snow is approximated with a constitutive equation inspired by the Prager-Drucker material model. The work function used here is

$$ \Psi(\mathbf{F_E}, \mathbf{F_P}) = \mu(\mathbf{F_P})||\mathbf{F_E}-\mathbf{R_E}||^2_{F}+\frac{\lambda(\mathbf{F_P})}{2}{(J_E-1)^2}$$

In order to enhance fracture and strain hardening induced by compression, the auther uses the following simple hardening rules for the bulk modulus and shear modulus: 

$$ \mu(\mathbf{F_P}) = \mu_0 e^{\xi(1-J_P)} \text{ and } \lambda(\mathbf{F_P}) = \lambda_0 e^{\xi(1-J_P)} $$

An interesting observation can be made regarding the original article by Stomakhin et al. The above model does not explicitly prescribes the fracturing locations inside the material, in contrast to other approaches such as work from Yue et al. Therefore, one must explicitly embed desired weak "fracture" locations inside the initial placement, since the fracture occurs in a smoothed out fashion following the plastic part of the deformation gradient. Although it is disputable whether the above actually captures the true mechanics in actual snow, it turns out that the above treatment successfully, and pratically produces visually stunning images.

<br><br>
<h5>2.4&emsp;Rasterization</h5>

Each node on the grid point has an interpolation kernel. Here, we employ a B-Spline kernel, which is a popular option for various mesh-free methods, such as Smoothed Particle Hydrodynamics (SPH). The interpolation functions are defined as : 

$$
N(q) = \left\{\begin{array}{lr}
        \frac{1}{2}|q|^3 - |q|^2 + \frac{2}{3},   & 0\leq q < 1\\
        -\frac{1}{6}|q|^3+|q|^2-2|q|+\frac{4}{3}, & 1\leq q < 2\\
        0, &\text{otherwise}
        \end{array}\right\}
$$

Where q is the $x,y$ or $z$th component of $\mathbf{x - x_i}/h$, and $\mathbf{x_i}$ indicates the spatial location of the grid node $i$. And we can also define the gradients :

$$
\nabla_{\mathbf{x}} N = \frac{1}{h} \nabla_{\mathbf{q}} N
$$

Note the $\frac{1}{h}$ comes from the chain rule.

The kernel centered around the grid node $i$ is then defined as

$$
w_\mathbf{i}(\tilde{\mathbf{x}}) = N(\tilde{x} - x_\mathbf{i})\cdot N(\tilde{y} - y_\mathbf{i})\cdot N(\tilde{z} - z_\mathbf{i})
$$


Using the kernel defined above, to rasterize a physical property $a_p$ of particle $p$ onto a gridnode, we then can simply compute

$$ a_\mathbf{i} = \Sigma_{p} a_p N(\mathbf{x}_p) $$

where $a_\mathbf{i}$ is the rasterized quantity, $a_p$ is the physical quantity defined on the material particle, and $\mathbf{x}_p$ is the spatial coordinate of the material point.

<h5>2.4&emsp;Complete Algorithm</h5>

    <div align="center">
    <table style="width=100%">
      <tr>
        <td align="middle">
        <img src="images/mpm_algorithm.png" width=40% />
        <div class="caption">Image Credit : <i>Stomakhin et al, 2013</i></div>
        </td>
      </tr>
    </table>
  </div>  

The complete procedure regarding the Material Point method can be summarized as the following: 
<ol>
  <li>Rasterize particle data onto the grid.</li>
  <li>Compute grid forces (based on Cauchy-Stress) with the rasterized data and the current deformation gradient.</li>
  <li>Update velocities on grid, based on the grid forces calculated above.</li>
  <li>Process geometry interactions, and apply it to the grid velocities.</li>
  <li>Solve for semi-implicit velocity updates.</li>
  <li>Update deformation gradients on each particle, by calculating the velocity gradients on the grid nodes.</li>
  <li>Update the particle velocities, using a combined FLIP / PIC interpolation scheme.</li>
  <li>Update the particle positions.</li>
</ol>

&emsp;For brievity, in this report, only the critical concepts of the algorithm were explained in detail. One can refer to the original paper by Stomakhin or the PhD thesis by Jiang for more rigorous background details.
<br><br>Also, the implicit update part requires computing the hessian of the work function with respect to the deformation gradient, and can be a painful task to implement in practice. Although a painstaking effort was put into the code in order to correctly implement this feature, we will skip the explanation of the work regarding the implicit time stepping scheme. The scheme is fully functional in our code and allows us to take relatively larger timesteps compared to explicit time stepping.

<br><br>
<h4>3&emsp;RENDERING</h4>

For visualization, we use the open-sourced raytracing software Mitsuba. The density data from the grid is used for volumetric rendering, and on top of that we also render the individual particles for a more clearer visualization of the boundaries. It was particularly tricky to get this to look good in 3D. This will be further discussed in the final report.

<br><br>
<h4>4&emsp;EXAMPLES</h4>

<h5>4.1&emsp;Crack Initiation and Initial Particle Placements</h5>

In the material point method, the materials points (the particles) are analogous to the "Gauss Points" in FEM. Naturally, one would regard that using more material points would result in a more accurate simulation.<br><br>
While this is true, there is an implementation detail that engineers should be aware of. The more material points we use to represent a certain geometry, the numerical representation of a certain geometry becomes more homogenized. This means the possible crack initiation points become more scarce. This is the main reason why many students implementing MPM schemes to produce snow animations often fail to generate nice-looking scenes; there is no specific guideline in the original article on how to sample the geometry using the material points.<br><br>

<h5>4.2&emsp;2D Examples - Test Cases</h5>

    <div align="center">
    <table style="width=20%">
      <tr>
        <td align="middle">
        <img src="images/input_5000_homo.png" width=100% />
        <div class="caption"><i>1. Rejection Sampled Snowball, 5000 Particles</i></div>
        </td>
        <td align="middle">
        <img src="images/input_30000_homo.png" width=100% />
        <div class="caption"><i>2. Rejection Sampled Snowball, 30000 Particles</i></div>
        </td>
        <td align="middle">
        <img src="images/input_30000_irreg_circle.png" width=100% />
        <div class="caption"><i>3. Rejection Sampled Snowball with embedded circular voids, 30000 Particles</i></div>
        </td>
        <td align="middle">
        <img src="images/input_30000_irreg_ellips.png" width=100% />
        <div class="caption"><i>4. Rejection Sampled Snowball with embedded elliptical voids, 30000 Particles</i></div>
      </tr>
    </table>
  </div>  

Here, we present 4 test cases presented in the above figures. Notice that for the snowball with only 5000 particles, there are not enough samples to make the snowball regular enough - thus more potential crack initiation locations.

<h5>4.3&emsp;2D Examples - Results</h5>
    <div align="center">
      <iframe width="960" height="600" src="https://www.youtube.com/embed/Uyjt-S9ad1Y" frameborder="0" allowfullscreen></iframe>
    </div>

    <div align="center">
    <table style="width=20%">
      <tr>
        <td align="middle">
        <img src="images/5000_homo.png" width=100% />
        <div class="caption"><i>1. Rejection Sampled Snowball, 5000 Particles, After Impact</i></div>
        </td>
        <td align="middle">
        <img src="images/30000_homo.png" width=100% />
        <div class="caption"><i>2. Rejection Sampled Snowball, 30000 Particles, After Impact</i></div>
        </td>
        <td align="middle">
        <img src="images/30000_circle.png" width=100% />
        <div class="caption"><i>3. Rejection Sampled Snowball with embedded circular voids, 30000 Particles, After Impact</i></div>
        </td>
        <td align="middle">
        <img src="images/30000_ellips.png" width=100% />
        <div class="caption"><i>4. Rejection Sampled Snowball with embedded elliptical voids, 30000 Particles, After Impact/i></div>
      </tr>
    </table>
  </div>  
<br><br>
Notice that the snowball with homogeneous 30k particles does not show dramatic fragmentation when compared to the 5k particle case. Also notice that the snowballs that were intentionally generated with embedded voids generate much finer fragments.



<h5>4.4&emsp;2D Examples - Test Cases</h5>

As a first example, we show a snowball smashing into a wall. We followed the parameters stated in Stomakhin et al: $$E_0 = 1.4 \times 10^5, \theta_c = 2.5 \times 10^{-2}, \theta_s = 7.5 \times 10^{-3}, \xi = 10$$, with initial particle placements random rejected sampled to a sphere.


    <div align="center">
    <table style="width=100%">
      <tr>
        <td align="middle">
        <img src="images/snowball1.png" width=100% />
        </td>
        <td align="middle">
        <img src="images/snowball2.png" width=100% />
        </td>
        <td align="middle">
        <img src="images/snowball3.png" width=100% />
        </td>
      </tr>
    </table>
  </div>  

<br><br>
<h4>5&emsp;EFFECT OF INITIAL PARTICLE PLACEMENTS</h4>

This will be further discussed in the final version.

<br><br>
<h4>6&emsp;REPORT, SLIDES, VIDEO</h4>

<a href="https://github.com/changyoonpark/mpm/blob/master/finproj.pdf"> Report</a>
<br>
<a href="https://drive.google.com/file/d/0By6WwzVVZzktQ3lQZGRPYW9JV2c/view?usp=sharing">Slides</a>
<br>
<a href="https://youtu.be/mNk1V3WUGZc">Introduction Video</a>
<br>
<a href="https://www.youtube.com/watch?v=om3VhdoDeT4">Simulation Animation Only</a>
<br>
<a href="https://github.com/changyoonpark/mpm">MPM Implementation Repository</a>
<br><br>
<hr>

<sub>Permission to make digital or hard copies of all or part of this work for personal or classroom use is granted without fee provided that copies are not made or distributed for pro t or commercial advantage and that copies bear this notice and the full citation on the  rst page. Copyrights for components of this work owned by others than ACM must be honored. Abstracting with credit is permitted. To copy otherwise, or republish, to post on servers or to redistribute to lists, requires prior specific permission and/or a fee. Request permissions from permissions@acm.org.<br>
&copy; 2017 Association for Computing Machinery. 
<br>Manuscript submitted to ACM</sub>
