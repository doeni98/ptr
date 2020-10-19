<div style="width=100%; border-bottom: 1px solid black;">
  <img src="https://upload.wikimedia.org/wikipedia/commons/e/e5/HEIG-VD_Logo_83x25_CMJN_ROUGE.svg"  height="80px" style="margin-bottom:5px;"/>
  <img src="https://www.hes-so.ch/data/images/accroches/logo-hes-so-couleur-363-7988.jpg" height="80px" style="float:right;"/>
</div>


<div style=" text-align: center; clear right; line-height: 0.5;margin-top:200px;">
  <p style="font-size: 40px;">
    Laboratoire 03 - PTR
  </p>
  <p style="font-size:25px;">
    Linux et Xenomai
  </p>
</div>

<div style=" text-align: center; clear right; line-height: 0.5; margin-top:300px;">
  <p style="font-size: 20px; font-weight:bold">Auteur:</p>
  <p style="font-size:20px;">Denis Bourqui</p>
  <p style="font-size: 20px; font-weight:bold">Professeur:</p>
  <p style="font-size:20px;">Yann Thoma</p>
  </br>
  <p style="font-size: 16px; font-weight:bold">Salle:</p>
  <p style="font-size:16px;">A07</p>
</div>


<div style="page-break-after: always"></div>
## Priorité des threads et Linux
Sans dohell on arrive à une vitesse de boucle de 445'000'000 cycles par secondes. Avec le dohell par contre on est pratiquement à la moitié. (200'000'000 cycles par secondes)

### Mesure d'une tache périodique avec nanosleep

Une tache à haute prio faisant une boucle qui attend 100ms à chaque itération arrive env **50 à 100 microsecondes**  en retard à chaque itération.