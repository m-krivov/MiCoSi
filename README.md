### What is MiCoSi?

**Mi**tosis **Co**mputer **Si**mulator is the software for computer modeling of a dividing cell during prometaphase, metaphase and anaphase. The model describes the entire cell and encompasses such processes as the divergence of spindle poles, the growth of microtubules, their linking to kinetochore and chromosome segregation. The numerical calculations can be performed on PC, GPU and even on supercomputers.

The aim of the project is to create a comprehensive tool that combines various, even competing models of the cellular mitotic machine into a complex one. With the help of MiCoSi, a researcher can describe an arbitrary cell, determine the nature of the interaction of its parts, and observe the virtual experiment. MiCoSi can be used to test new micro-level models, but on a cell-wide scale. MiCoSi also allows you to explore how the biophysical parameters of the cell affect the dynamics of mitosis.

### Roadmap

*  Documentation, development of configuration wizards _(planned)_
*  The study of chromosome segregation _(planned)_
*  Inclusion of third-party models _(in progress)_
*  Source code refactoring and publishing _(in progress)_
*  Investigation of merotelic kinetochores _(completed)_
*  Porting to GPUs and supercomputers _(completed)_
*  Development of a visualizer and API for .NET _(completed)_
*  Development of our own model _(completed)_

### Versions

* _v0.9.x_ – legacy source code after its refactoring and adaptation
* _legacy_ – private builds used for ad hoc computations in 2015-2018

### Acknowledgments

Prof. Fazli Ataullakhanov is the ideologist and curator of this project. We are more than thankful to him and his team for their comprehensive support and assistance in the development of the mathematical model.

The study was carried out with the financial support from Russian Foundation for Basic Research within research projects 16-07-01064a and 19-07-01164a.

The project is implemented using the equipment of the Center for Collective Use of Super High-Performance Computing Resources of the M.V.Lomonosov Moscow State University.

### Publications

1. Krivov, M.A., Ataullakhanov, F.I. & Ivanov, P.S. (2021) *Computer simulation of merotelic kinetochore-microtubule attachments: corona size is more important than other cell parameters.* Chromosome Res. [DOI](https://doi.org/10.1007/s10577-021-09669-y), [TEXT](https://link.springer.com/epdf/10.1007/s10577-021-09669-y?sharing_token=_aWzQmjgswy5kq23tg2FQve4RwlQNchNByi7wbcMAY4CN927pNYtjm3r85QKOujT5yMMMffsQQ7sXJbCgKpn5c-STa_dIaf74YIVSRFMCWjsQAvJjNDEh_bSK73cWnf0MhsNzZ8Itcp2D8ms1mWqdJQpjBSY_dmDataVluBVBaI%3D)
2. Krivov, M.A., Iroshnikov, N.G., et al. (2021) *Comparison of AMD Zen 2 and Intel Cascade Lake on the Task of Modeling the Mammalian Cell Division*. In: Mathematical Modeling and Supercomputer Technologies (MMST 2020). Communications in Computer and Information Science, vol 1413, pp. 320-333 [DOI](https://doi.org/10.1007/978-3-030-78759-2_27)
3. Krivov, M.A., Ataullakhanov, F.I. & Ivanov, P.S. (2021) *Evaluation of the Effect of Cell Parameters on the Number of Microtubule Merotelic Attachments in Metaphase Using a Three-Dimensional Computer Model.* In: Practical Applications of Computational Biology & Bioinformatics, 14th International Conference (PACBB 2020). Advances in Intelligent Systems and Computing, vol 1240, pp. 144 — 154, [DOI](http://dx.doi.org/10.1007/978-3-030-54568-0_15)

<details>
  <summary>Other publications</summary>
  <br/>
  
4. Krivov, M.A., and Ivanov, P.S. (2019). *MiCoSi: open software for simulation of prometaphase, metaphase and anaphase of mitosis.* In: Current Topics in Computational and Applied Mathematics. Proc. Int. Conf. (1-5 July, 2019, Novosibirsk, Russia), pp. 246-252, [PDF](http://dx.doi.org/10.24411/9999-016A-2019-10040)  
5. Krivov, M.A., Zaytsev, A.V., et al. (2018). *Modeling the Division of Biological Cells in the Stage of Metaphase on the “Lomonosov-2” Supercomputer.* Comput. Methods Software Dev., vol. 19, pp. 327 — 339, [PDF](http://num-meth.srcc.msu.ru/zhurnal/tom_2018/pdf/v19r430.pdf)  
6. Krivov, M.A., Zakharov, N.Yu., et al. (2016). *Software for simulation of the biological cells division on GPUs.* In: Russian Supercomputer Days. Proc. Int. Conf. (26-27 Sept., 2016, Moscow), pp. 582–588, [PDF](http://2016.russianscdays.org/files/pdf16/582.pdf)
</details>
