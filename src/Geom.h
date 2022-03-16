typedef RE::NiPoint3 Point3;

/// <summary>
/// rotates vector V to rotation vector
/// </summary>
/// <param name="r">V = (r, 0, 0)</param>
/// <param name="rotation">vert (pi/2..-pi/2), 0, horiz (0..2pi, clock, 0 in OY)</param>
Point3 rotate(float r, const Point3& rotation);

/// <summary>
/// rotates vector V to rotation vector (uses only Z)
/// </summary>
Point3 rotateZ(float r, const Point3& rotation);

/// <summary>
/// get distanse between segment (A, B) and point C
/// </summary>
float dist(const Point3& A, const Point3& B, const Point3& C);

/// <summary>
/// get distanse between segments (A, B) and (C, D)
/// </summary>
float dist(const Point3& A, const Point3& B, const Point3& C, const Point3& D);
