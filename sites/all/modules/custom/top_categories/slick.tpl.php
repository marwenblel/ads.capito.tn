<section class="center-slick slider">
    <div class="category">
        <img src="<?php echo $variables['slick']['images'];?>/cars.png">
		<div><center>Voitures</center></div>
    </div>
    <div class="category">
        <img src="<?php echo $variables['slick']['images'];?>/vetements.png">
		<div><center>Vêtements</center></div>
    </div>
    <div class="category">
        <img  src="<?php echo $variables['slick']['images'];?>/immobiliers.png">
      <div><center>Immobiliers</center></div>
    </div>
    <div class="category">
        <img  src="<?php echo $variables['slick']['images'];?>/emploi.png">
      <div><center>Emploi</center></div>
    </div>
    <div class="category">
        <img  src="<?php echo $variables['slick']['images'];?>/ordinateurs.png">
      <div><center>Ordinateurs</center></div>
    </div>
    <div class="category">
        <img  src="<?php echo $variables['slick']['images'];?>/vacances.png">
      <div><center>Vacances</center></div>
    </div>
    <div class="category">
        <img src="<?php echo $variables['slick']['images'];?>/livres.png">
      <div><center>Livres</center></div>
    </div>
    <div class="category">
        <img  src="<?php echo $variables['slick']['images'];?>/decoration.png">
      <div><center>Décoration</center></div>
    </div>
    <div class="category">
        <img  src="<?php echo $variables['slick']['images'];?>/ameublement.png">
      <div><center>Ameublement</center></div>
    </div>
</section>

<script src="https://code.jquery.com/jquery-2.2.0.min.js" type="text/javascript"></script>
<script src="<?php echo $variables['slick']['path'];?>/slick.js" type="text/javascript"></script>
<script type="text/javascript">
    $(document).on('ready', function() {

        $(".center-slick").slick({
            dots: true,
            infinite: true,
            centerMode: true,
            slidesToShow: 5,
            slidesToScroll: 3
        });
    });
</script>