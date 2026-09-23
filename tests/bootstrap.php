<?php

use SebastianBergmann\Comparator\Factory;
use Tensor\Tests\TensorComparator;

require __DIR__ . '/../vendor/autoload.php';

Factory::getInstance()->register(
    new TensorComparator()
);
